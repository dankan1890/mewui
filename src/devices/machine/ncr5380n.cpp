// license:BSD-3-Clause
// copyright-holders:R. Belmont, Olivier Galibert
/*********************************************************************

    ncr5380n.c

    Implementation of the NCR 5380, aka the Zilog Z5380 & AMD Am5380

    TODO:
    - IRQs
    - Target mode
    - NMOS/CMOS functional differences
    - Timings should not be clock-based (5380 has no clock input)

    40801766 - IIx ROM waiting point for "next read fails"

*********************************************************************/

#include "emu.h"
#include "ncr5380n.h"

DEFINE_DEVICE_TYPE(NCR5380N, ncr5380n_device, "ncr5380_new", "NCR 5380 SCSI (new)")
DEFINE_DEVICE_TYPE(NCR53C80, ncr53c80_device, "ncr53c80", "NCR 53C80 SCSI")

void ncr5380n_device::map(address_map &map)
{
	map(0x0, 0x0).rw(FUNC(ncr5380n_device::scsidata_r), FUNC(ncr5380n_device::outdata_w));
	map(0x1, 0x1).rw(FUNC(ncr5380n_device::icmd_r), FUNC(ncr5380n_device::icmd_w));
	map(0x2, 0x2).rw(FUNC(ncr5380n_device::mode_r), FUNC(ncr5380n_device::mode_w));
	map(0x3, 0x3).rw(FUNC(ncr5380n_device::command_r), FUNC(ncr5380n_device::command_w));
	map(0x4, 0x4).rw(FUNC(ncr5380n_device::status_r), FUNC(ncr5380n_device::selenable_w));
	map(0x5, 0x5).rw(FUNC(ncr5380n_device::busandstatus_r), FUNC(ncr5380n_device::startdmasend_w));
	map(0x6, 0x6).rw(FUNC(ncr5380n_device::indata_r), FUNC(ncr5380n_device::startdmatargetrx_w));
	map(0x7, 0x7).rw(FUNC(ncr5380n_device::resetparityirq_r), FUNC(ncr5380n_device::startdmainitrx_w));
}

ncr5380n_device::ncr5380n_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
	: nscsi_device(mconfig, type, tag, owner, clock)
	, nscsi_slot_card_interface(mconfig, *this, DEVICE_SELF)
	, m_fake_clock(10000000)
	, tm(nullptr), status(0), istatus(0), m_mode(0)
	, m_outdata(0), m_busstatus(0), m_dmalatch(0), m_icommand(0), m_tcommand(0), clock_conv(0), sync_offset(0), sync_period(0), bus_id(0), select_timeout(0)
	, seq(0), tcount(0), mode(0), state(0), irq(false), drq(false)
	, m_irq_handler(*this)
	, m_drq_handler(*this)
{
}

ncr5380n_device::ncr5380n_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: ncr5380n_device(mconfig, NCR5380N, tag, owner, clock)
{
}

ncr53c80_device::ncr53c80_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: ncr5380n_device(mconfig, NCR53C80, tag, owner, clock)
{
}

void ncr5380n_device::device_start()
{
	save_item(NAME(m_tcommand));
	save_item(NAME(m_icommand));
	save_item(NAME(status));
	save_item(NAME(istatus));
	save_item(NAME(m_busstatus));
	save_item(NAME(tcount));
	save_item(NAME(mode));
	save_item(NAME(irq));
	save_item(NAME(drq));
	save_item(NAME(clock_conv));
	save_item(NAME(m_dmalatch));

	m_irq_handler.resolve_safe();
	m_drq_handler.resolve_safe();

	tcount = 0;
	status = 0;
	bus_id = 0;
	select_timeout = 0;
	tm = timer_alloc(0);
}

void ncr5380n_device::device_reset()
{
	clock_conv = 2;
	sync_period = 5;
	sync_offset = 0;
	seq = 0;
	status = 0;
	m_tcommand = 0;
	m_icommand = 0;
	istatus = 0;
	m_busstatus = 0;
	irq = false;
	m_irq_handler(irq);
	reset_soft();
}

void ncr5380n_device::reset_soft()
{
	state = IDLE;
	scsi_bus->ctrl_w(scsi_refid, 0, S_ALL); // clear any signals we're driving
	scsi_bus->ctrl_wait(scsi_refid, S_ALL, S_ALL);
	status = 0;
	drq = false;
	m_drq_handler(drq);
	reset_disconnect();
}

void ncr5380n_device::reset_disconnect()
{
	mode = MODE_D;
}

//static int last_phase = -1;

void ncr5380n_device::scsi_ctrl_changed()
{
	uint32_t ctrl = scsi_bus->ctrl_r();

//  logerror("scsi_ctrl_changed: lines now %x\n", ctrl);

/*  if ((ctrl & (S_PHASE_MASK|S_SEL|S_BSY)) != last_phase)
    {
        logerror("phase now %d, REQ %x SEL %x BSY %x\n", ctrl & S_PHASE_MASK, ctrl & S_REQ, ctrl & S_SEL, ctrl & S_BSY);
        last_phase = (S_PHASE_MASK|S_SEL|S_BSY);
    }*/

	// recalculate phase match
	m_busstatus &= ~BAS_PHASEMATCH;
	if ((ctrl & S_PHASE_MASK) == (m_tcommand & S_PHASE_MASK))
	{
		m_busstatus |= BAS_PHASEMATCH;
	}

	if (m_mode & MODE_DMA)
	{
		// if BSY drops or the phase goes mismatch, that terminates the DMA
		if ((!(ctrl & S_BSY)) || !(m_busstatus & BAS_PHASEMATCH))
		{
//          logerror("BSY dropped or phase mismatch during DMA, ending DMA\n");
			m_mode &= ~MODE_DMA;
			m_busstatus |= BAS_ENDOFDMA;
			drq_clear();
		}
	}

	if(ctrl & S_RST) {
		logerror("%s: scsi bus reset\n", tag());
		return;
	}

	step(false);
}

void ncr5380n_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	step(true);
}

void ncr5380n_device::step(bool timeout)
{
	uint32_t ctrl = scsi_bus->ctrl_r();
	uint32_t data = scsi_bus->data_r();

	if(0)
		logerror("%s: state=%d.%d %s\n",
					tag(), state & STATE_MASK, (state & SUB_MASK) >> SUB_SHIFT,
					timeout ? "timeout" : "change");

	if(mode == MODE_I && !(ctrl & S_BSY)) {
		state = IDLE;
		reset_disconnect();
		check_irq();
	}
	switch(state & SUB_MASK ? state & SUB_MASK : state & STATE_MASK) {
	case IDLE:
		break;

	case ARB_COMPLETE << SUB_SHIFT: {
		if(!timeout)
			break;

		int win;
		for(win=7; win>=0 && !(data & (1<<win)); win--) {};
//      logerror("arb complete: data %02x win %02x scsi_id %02x\n", data, win, scsi_id);
		if(win != scsi_id) {
			scsi_bus->data_w(scsi_refid, 0);
			scsi_bus->ctrl_w(scsi_refid, 0, S_ALL);
			fatalerror("need to wait for bus free\n");
		}

		state &= STATE_MASK;
		step(true);
		break;
	}

	case SEND_WAIT_SETTLE << SUB_SHIFT:
		if(!timeout)
			break;

		state = (state & STATE_MASK) | (SEND_WAIT_REQ_0 << SUB_SHIFT);
		step(false);
		break;

	case SEND_WAIT_REQ_0 << SUB_SHIFT:
		if(ctrl & S_REQ)
			break;
		state = state & STATE_MASK;
		scsi_bus->data_w(scsi_refid, 0);
		scsi_bus->ctrl_w(scsi_refid, 0, S_ACK);
		step(false);

		// byte's done, ask for another if the target hasn't said otherwise
		if (m_mode & MODE_DMA)
		{
			drq_set();
		}
		break;

	case RECV_WAIT_REQ_1 << SUB_SHIFT:
		if(!(ctrl & S_REQ))
			break;

		state = (state & STATE_MASK) | (RECV_WAIT_SETTLE << SUB_SHIFT);
		delay_cycles(sync_period);
		break;

	case RECV_WAIT_SETTLE << SUB_SHIFT:
		if(!timeout)
			break;

		m_dmalatch = scsi_bus->data_r();
		scsi_bus->ctrl_w(scsi_refid, S_ACK, S_ACK);
		state = (state & STATE_MASK) | (RECV_WAIT_REQ_0 << SUB_SHIFT);
		step(false);
		break;

	case RECV_WAIT_REQ_0 << SUB_SHIFT:
		if(ctrl & S_REQ)
			break;
		state = state & STATE_MASK;
		step(false);

		drq_set();  // raise DRQ now that we've completed
		break;

	default:
		logerror("%s: step() unexpected state %d.%d\n",
					tag(),
					state & STATE_MASK, (state & SUB_MASK) >> SUB_SHIFT);
		exit(0);
	}
}

void ncr5380n_device::send_byte()
{
	state = (state & STATE_MASK) | (SEND_WAIT_SETTLE << SUB_SHIFT);
	scsi_bus->data_w(scsi_refid, m_dmalatch);

	scsi_bus->ctrl_w(scsi_refid, S_ACK, S_ACK);
	scsi_bus->ctrl_wait(scsi_refid, S_REQ, S_REQ);
	delay_cycles(sync_period);
}

void ncr5380n_device::recv_byte()
{
	state = (state & STATE_MASK) | (RECV_WAIT_REQ_1 << SUB_SHIFT);
	step(false);
}

void ncr5380n_device::function_bus_complete()
{
	state = IDLE;
//  istatus |= I_FUNCTION|I_BUS;
	check_irq();
}

void ncr5380n_device::function_complete()
{
	state = IDLE;
//  istatus |= I_FUNCTION;
	check_irq();
}

void ncr5380n_device::bus_complete()
{
	state = IDLE;
//  istatus |= I_BUS;
	check_irq();
}

void ncr5380n_device::delay(int cycles)
{
	if(!clock_conv)
		return;
	cycles *= clock_conv;
	tm->adjust(attotime::from_ticks(cycles, m_fake_clock));
}

void ncr5380n_device::delay_cycles(int cycles)
{
	tm->adjust(attotime::from_ticks(cycles, m_fake_clock));
}

uint8_t ncr5380n_device::scsidata_r()
{
	return scsi_bus->data_r();
}

void ncr5380n_device::outdata_w(uint8_t data)
{
	m_outdata = data;

	// are we driving the data bus?
	if (m_icommand & IC_DBUS)
	{
		scsi_bus->data_w(scsi_refid, data);
	}
}

uint8_t ncr5380n_device::icmd_r()
{
	return m_icommand;
}

void ncr5380n_device::icmd_w(uint8_t data)
{
	// asserting to drive the data bus?
	if ((data & IC_DBUS) && !(m_icommand & IC_DBUS))
	{
//      logerror("%s: driving data bus with %02x\n", tag(), m_outdata);
		scsi_bus->data_w(scsi_refid, m_outdata);
		delay(2);
	}

	// any control lines changing?
	uint8_t mask = (data & IC_PHASEMASK) ^ (m_icommand & IC_PHASEMASK);
	if (mask)
	{
		// translate data to nscsi
		uint8_t newdata;

		newdata = (data & IC_RST ? S_RST : 0) |
			(data & IC_ACK ? S_ACK : 0) |
			(data & IC_BSY ? S_BSY : 0) |
			(data & IC_SEL ? S_SEL : 0) |
			(data & IC_ATN ? S_ATN : 0);

//      logerror("%s: changing control lines %04x\n", tag(), newdata);
		scsi_bus->ctrl_w(scsi_refid, newdata, S_RST|S_ACK|S_BSY|S_SEL|S_ATN);
	}

	m_icommand = (data & IC_WRITEMASK);
	delay(2);
}

uint8_t ncr5380n_device::mode_r()
{
	return m_mode;
}

void ncr5380n_device::mode_w(uint8_t data)
{
//  logerror("%s: mode_w %02x (%s)\n", tag(), data, machine().describe_context());
	// arbitration bit being set?
	if ((data & MODE_ARBITRATE) && !(m_mode & MODE_ARBITRATE))
	{
		// if SEL is selected and the assert SEL bit in the initiator
		// command register is clear, fail
		if ((scsi_bus->ctrl_r() & S_SEL) && !(m_icommand & IC_SEL))
		{
			m_icommand |= IC_ARBLOST;
		}
		else
		{
			seq = 0;
//          state = DISC_SEL_ARBITRATION;
			arbitrate();
		}
	}
	else if (!(data & MODE_ARBITRATE) && (m_mode & MODE_ARBITRATE))
	{
		// arbitration in progress bit ONLY clears when the host disables arbitration. (thanks, Zilog Z8530 manual!)
		// the Apple II High Speed SCSI Card boot code explicitly requires this.
		m_icommand &= ~ IC_ARBITRATION;
	}
	m_mode = data;
}

uint8_t ncr5380n_device::command_r()
{
//  logerror("%s: command_r %02x (%s)\n", tag(), m_tcommand, machine().describe_context());
	return m_tcommand;
}

void ncr5380n_device::command_w(uint8_t data)
{
//  logerror("%s: command_w %02x (%s)\n", tag(), data, machine().describe_context());
	m_tcommand = data;

	// recalculate phase match
	m_busstatus &= ~BAS_PHASEMATCH;
	if ((scsi_bus->ctrl_r() & S_PHASE_MASK) == (m_tcommand & S_PHASE_MASK))
	{
		m_busstatus |= BAS_PHASEMATCH;
	}
}

void ncr5380n_device::arbitrate()
{
	m_icommand &= ~IC_ARBLOST;
	m_icommand |= IC_ARBITRATION;   // set in progress flag
	state = (state & STATE_MASK) | (ARB_COMPLETE << SUB_SHIFT);
	scsi_bus->data_w(scsi_refid, m_outdata);
	scsi_bus->ctrl_w(scsi_refid, S_BSY, S_BSY);
	m_icommand |= IC_BSY;   // make sure BSY shows in icommand (Zilog 5380 manual suggests this behavior, Apple II High-Speed SCSI Card firmware requires it)
	delay(11);
}

void ncr5380n_device::check_irq()
{
	#if 0
	bool oldirq = irq;
	irq = istatus != 0;
	if(irq != oldirq)
		m_irq_handler(irq);
	#endif
}

uint8_t ncr5380n_device::status_r()
{
	uint32_t ctrl = scsi_bus->ctrl_r();
	uint8_t res = status |
		(ctrl & S_RST ? ST_RST : 0) |
		(ctrl & S_BSY ? ST_BSY : 0) |
		(ctrl & S_REQ ? ST_REQ : 0) |
		(ctrl & S_MSG ? ST_MSG : 0) |
		(ctrl & S_CTL ? ST_CD  : 0) |
		(ctrl & S_INP ? ST_IO  : 0) |
		(ctrl & S_SEL ? ST_SEL : 0);

//  logerror("%s: status_r %02x (%s)\n", tag(), res, machine().describe_context());
	return res;
}

void ncr5380n_device::selenable_w(uint8_t data)
{
}

uint8_t ncr5380n_device::busandstatus_r()
{
	uint32_t ctrl = scsi_bus->ctrl_r();
	uint8_t res = m_busstatus |
		(ctrl & S_ATN ? BAS_ATN : 0) |
		(ctrl & S_ACK ? BAS_ACK : 0);

//  logerror("%s: busandstatus_r %02x (%s)\n", tag(), res, machine().describe_context());

	return res;
}

void ncr5380n_device::startdmasend_w(uint8_t data)
{
	logerror("%02x to start dma send\n", data);
	drq_set();
}

uint8_t ncr5380n_device::indata_r()
{
	return dma_r();
}

void ncr5380n_device::startdmatargetrx_w(uint8_t data)
{
	logerror("%02x to start dma target Rx\n", data);
}

uint8_t ncr5380n_device::resetparityirq_r()
{
	return 0;
}

void ncr5380n_device::startdmainitrx_w(uint8_t data)
{
//  logerror("%02x to start dma initiator Rx\n", data);
	recv_byte();
}

void ncr5380n_device::dma_w(uint8_t val)
{
	// drop DRQ until we're ready for another byte
	drq_clear();

	if (m_mode & MODE_DMA)
	{
		m_dmalatch = val;
		send_byte();
	}
}

uint8_t ncr5380n_device::dma_r()
{
	if (!machine().side_effects_disabled())
	{
		// drop DRQ
		drq_clear();

		// set up to receive our next byte if still in DMA mode
		scsi_bus->ctrl_w(scsi_refid, 0, S_ACK);
		if (m_mode & MODE_DMA)
		{
			recv_byte();
		}
	}
	return m_dmalatch;
}

void ncr5380n_device::drq_set()
{
	if(!drq)
	{
		drq = true;
		m_busstatus |= BAS_DMAREQUEST;
		m_drq_handler(drq);
	}
}

void ncr5380n_device::drq_clear()
{
	if(drq)
	{
		drq = false;
		m_busstatus &= ~BAS_DMAREQUEST;
		m_drq_handler(drq);
	}
}

uint8_t ncr5380n_device::read(offs_t offset)
{
	switch (offset & 7)
	{
	case 0:
		return scsidata_r();

	case 1:
		return icmd_r();

	case 2:
		return mode_r();

	case 3:
		return command_r();

	case 4:
		return status_r();

	case 5:
		return busandstatus_r();

	case 6:
		return indata_r();

	case 7:
		return resetparityirq_r();
	}

	return 0xff;
}

void ncr5380n_device::write(offs_t offset, uint8_t data)
{
//  logerror("%x to 5380 @ %x\n", data, offset);
	switch (offset & 7)
	{
	case 0:
		outdata_w(data);
		break;

	case 1:
		icmd_w(data);
		break;

	case 2:
		mode_w(data);
		break;

	case 3:
		command_w(data);
		break;

	case 4:
		selenable_w(data);
		break;

	case 5:
		startdmasend_w(data);
		break;

	case 6:
		startdmatargetrx_w(data);
		break;

	case 7:
		startdmainitrx_w(data);
		break;
	}
}
