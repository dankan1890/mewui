// license:BSD-3-Clause
// copyright-holders:Nathan Woods, Olivier Galibert, Miodrag Milanovic
/*********************************************************************



*********************************************************************/

#include "emu.h"
#include "zippath.h"
#include "floppy.h"
#include "formats/imageutl.h"

/*
    Debugging flags. Set to 0 or 1.
*/

// Show step operation
#define TRACE_STEP 0
#define TRACE_AUDIO 0

#define PITCH_SEEK_SAMPLES 1

#define FLOPSND_TAG "floppysound"

// device type definition
const device_type FLOPPY_CONNECTOR = &device_creator<floppy_connector>;

// generic 3" drives
const device_type FLOPPY_3_SSDD = &device_creator<floppy_3_ssdd>;
const device_type FLOPPY_3_DSDD = &device_creator<floppy_3_dsdd>;

// generic 3.5" drives
const device_type FLOPPY_35_SSDD = &device_creator<floppy_35_ssdd>;
const device_type FLOPPY_35_DD = &device_creator<floppy_35_dd>;
const device_type FLOPPY_35_HD = &device_creator<floppy_35_hd>;
const device_type FLOPPY_35_ED = &device_creator<floppy_35_ed>;

// generic 5.25" drives
const device_type FLOPPY_525_SSSD_35T = &device_creator<floppy_525_sssd_35t>;
const device_type FLOPPY_525_SD_35T = &device_creator<floppy_525_sd_35t>;
const device_type FLOPPY_525_SSSD = &device_creator<floppy_525_sssd>;
const device_type FLOPPY_525_SD = &device_creator<floppy_525_sd>;
const device_type FLOPPY_525_SSDD = &device_creator<floppy_525_ssdd>;
const device_type FLOPPY_525_DD = &device_creator<floppy_525_dd>;
const device_type FLOPPY_525_SSQD = &device_creator<floppy_525_ssqd>;
const device_type FLOPPY_525_QD = &device_creator<floppy_525_qd>;
const device_type FLOPPY_525_HD = &device_creator<floppy_525_hd>;

// generic 8" drives
const device_type FLOPPY_8_SSSD = &device_creator<floppy_8_sssd>;
const device_type FLOPPY_8_DSSD = &device_creator<floppy_8_dssd>;
const device_type FLOPPY_8_SSDD = &device_creator<floppy_8_ssdd>;
const device_type FLOPPY_8_DSDD = &device_creator<floppy_8_dsdd>;

// Epson 3.5" drives
#if 0
const device_type EPSON_SMD_110 = &device_creator<epson_smd_110>;
const device_type EPSON_SMD_120 = &device_creator<epson_smd_120>;
const device_type EPSON_SMD_125 = &device_creator<epson_smd_125>;
const device_type EPSON_SMD_130 = &device_creator<epson_smd_130>;
const device_type EPSON_SMD_140 = &device_creator<epson_smd_140>;
const device_type EPSON_SMD_150 = &device_creator<epson_smd_150>;
const device_type EPSON_SMD_160 = &device_creator<epson_smd_160>;
#endif
const device_type EPSON_SMD_165 = &device_creator<epson_smd_165>;
#if 0
const device_type EPSON_SMD_170 = &device_creator<epson_smd_170>;
const device_type EPSON_SMD_180 = &device_creator<epson_smd_180>;
const device_type EPSON_SMD_240L = &device_creator<epson_smd_240l>;
const device_type EPSON_SMD_280HL = &device_creator<epson_smd_280hl>;
const device_type EPSON_SMD_440L = &device_creator<epson_smd_440l>;
const device_type EPSON_SMD_449L = &device_creator<epson_smd_449l>;
const device_type EPSON_SMD_480LM = &device_creator<epson_smd_480lm>;
const device_type EPSON_SMD_489M = &device_creator<epson_smd_489m>;
#endif

// Epson 5.25" drives
#if 0
const device_type EPSON_SD_311 = &device_creator<epson_sd_311>;
#endif
const device_type EPSON_SD_320 = &device_creator<epson_sd_320>;
const device_type EPSON_SD_321 = &device_creator<epson_sd_321>;
#if 0
const device_type EPSON_SD_521L = &device_creator<epson_sd_531l>;
const device_type EPSON_SD_525 = &device_creator<epson_sd_525>;
const device_type EPSON_SD_543 = &device_creator<epson_sd_543>;
const device_type EPSON_SD_545 = &device_creator<epson_sd_545>;
const device_type EPSON_SD_560 = &device_creator<epson_sd_560>;
const device_type EPSON_SD_580L = &device_creator<epson_sd_580l>;
const device_type EPSON_SD_581L = &device_creator<epson_sd_581l>;
const device_type EPSON_SD_621L = &device_creator<epson_sd_621l>;
const device_type EPSON_SD_680L = &device_creator<epson_sd_680l>;
#endif

// Sony 3.5" drives
const device_type SONY_OA_D31V = &device_creator<sony_oa_d31v>;
const device_type SONY_OA_D32W = &device_creator<sony_oa_d32w>;
const device_type SONY_OA_D32V = &device_creator<sony_oa_d32v>;

// TEAC 5.25" drives
#if 0
const device_type TEAC_FD_55A = &device_creator<teac_fd_55a>;
const device_type TEAC_FD_55B = &device_creator<teac_fd_55b>;
#endif
const device_type TEAC_FD_55E = &device_creator<teac_fd_55e>;
const device_type TEAC_FD_55F = &device_creator<teac_fd_55f>;
const device_type TEAC_FD_55G = &device_creator<teac_fd_55g>;

// ALPS 5.25" drives
const device_type ALPS_3255190x = &device_creator<alps_3255190x>;


const floppy_format_type floppy_image_device::default_floppy_formats[] = {
	FLOPPY_D88_FORMAT,
	FLOPPY_DFI_FORMAT,
	FLOPPY_IMD_FORMAT,
	FLOPPY_IPF_FORMAT,
	FLOPPY_MFI_FORMAT,
	FLOPPY_MFM_FORMAT,
	FLOPPY_TD0_FORMAT,
	FLOPPY_CQM_FORMAT,
	FLOPPY_DSK_FORMAT,
	nullptr
};

floppy_connector::floppy_connector(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, FLOPPY_CONNECTOR, "Floppy drive connector abstraction", tag, owner, clock, "floppy_connector", __FILE__),
	device_slot_interface(mconfig, *this),
	formats(nullptr),
	m_enable_sound(false)
{
}

floppy_connector::~floppy_connector()
{
}

void floppy_connector::set_formats(const floppy_format_type *_formats)
{
	formats = _formats;
}

void floppy_connector::device_start()
{
}

void floppy_connector::device_config_complete()
{
	floppy_image_device *dev = dynamic_cast<floppy_image_device *>(get_card_device());
	if(dev)
	{
		dev->set_formats(formats);
		dev->enable_sound(m_enable_sound);
	}
}

floppy_image_device *floppy_connector::get_device()
{
	return dynamic_cast<floppy_image_device *>(get_card_device());
}

//-------------------------------------------------
//  floppy_image_device - constructor
//-------------------------------------------------

floppy_image_device::floppy_image_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source)
	: device_t(mconfig, type, name, tag, owner, clock, shortname, source),
		device_image_interface(mconfig, *this),
		device_slot_card_interface(mconfig, *this),
		input_format(nullptr),
		output_format(nullptr),
		image(nullptr),
		fif_list(nullptr),
		index_timer(nullptr),
		tracks(0),
		sides(0),
		form_factor(0),
		motor_always_on(false),
		dir(0), stp(0), wtg(0), mon(0), ss(0), idx(0), wpt(0), rdy(0), dskchg(0),
		ready(false),
		rpm(0),
		floppy_ratio_1(0),
		revolution_count(0),
		cyl(0),
		subcyl(0),
		image_dirty(false),
		ready_counter(0),
		m_make_sound(false),
		m_sound_out(nullptr)
{
	extension_list[0] = '\0';
	m_err = IMAGE_ERROR_INVALIDIMAGE;
}

//-------------------------------------------------
//  floppy_image_device - destructor
//-------------------------------------------------

floppy_image_device::~floppy_image_device()
{
	for(floppy_image_format_t *format = fif_list; format; ) {
		floppy_image_format_t* tmp_format = format;
		format = format->next;
		delete tmp_format;
	}
	fif_list = nullptr;
}

void floppy_image_device::setup_load_cb(load_cb cb)
{
	cur_load_cb = cb;
}

void floppy_image_device::setup_unload_cb(unload_cb cb)
{
	cur_unload_cb = cb;
}

void floppy_image_device::setup_index_pulse_cb(index_pulse_cb cb)
{
	cur_index_pulse_cb = cb;
}

void floppy_image_device::setup_ready_cb(ready_cb cb)
{
	cur_ready_cb = cb;
}

void floppy_image_device::setup_wpt_cb(wpt_cb cb)
{
	cur_wpt_cb = cb;
}

void floppy_image_device::set_formats(const floppy_format_type *formats)
{
	extension_list[0] = '\0';
	fif_list = nullptr;
	for(int cnt=0; formats[cnt]; cnt++)
	{
		// allocate a new format
		floppy_image_format_t *fif = formats[cnt]();
		if(!fif_list)
			fif_list = fif;
		else
			fif_list->append(fif);

		add_format(fif->name(), fif->description(), fif->extensions(), "");

		image_specify_extension( extension_list, 256, fif->extensions() );
	}

	// set brief and instance name
	update_names();
}

floppy_image_format_t *floppy_image_device::get_formats() const
{
	return fif_list;
}

floppy_image_format_t *floppy_image_device::get_load_format() const
{
	return input_format;
}

void floppy_image_device::device_config_complete()
{
	update_names();
}

void floppy_image_device::set_rpm(float _rpm)
{
	if(rpm == _rpm)
		return;

	rpm = _rpm;
	rev_time = attotime::from_double(60/rpm);
	floppy_ratio_1 = int(1000.0f*rpm/300.0f+0.5f);
}

void floppy_image_device::setup_write(floppy_image_format_t *_output_format)
{
	output_format = _output_format;
	commit_image();
}

void floppy_image_device::commit_image()
{
	image_dirty = false;
	if(!output_format || !output_format->supports_save())
		return;
	io_generic io;
	// Do _not_ remove this cast otherwise the pointer will be incorrect when used by the ioprocs.
	io.file = (device_image_interface *)this;
	io.procs = &image_ioprocs;
	io.filler = 0xff;

	osd_file::error err = image_core_file().truncate(0);
	if (err != osd_file::error::NONE)
		popmessage("Error, unable to truncate image: %d", int(err));

	output_format->save(&io, image);
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void floppy_image_device::device_start()
{
	rpm = 0;
	motor_always_on = false;

	idx = 0;

	/* motor off */
	mon = 1;

	cyl = 0;
	subcyl = 0;
	ss  = 0;
	stp = 1;
	wpt = 0;
	dskchg = exists() ? 1 : 0;
	index_timer = timer_alloc(0);
	image_dirty = false;
	ready = true;
	ready_counter = 0;

	setup_characteristics();

	if (m_make_sound) m_sound_out = subdevice<floppy_sound_device>(FLOPSND_TAG);

	save_item(NAME(cyl));
	save_item(NAME(subcyl));
}

void floppy_image_device::device_reset()
{
	if (m_make_sound)
	{
		// Have we loaded all samples? Otherwise mute the floppy.
		m_make_sound = m_sound_out->samples_loaded();
	}

	revolution_start_time = attotime::never;
	revolution_count = 0;
	mon = 1;
	if(!ready) {
		ready = true;
		if(!cur_ready_cb.isnull())
			cur_ready_cb(this, ready);
	}
	if(motor_always_on)
		mon_w(0);
}

void floppy_image_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	index_resync();
}

floppy_image_format_t *floppy_image_device::identify(std::string filename)
{
	util::core_file::ptr fd;
	std::string revised_path;

	osd_file::error err = util::zippath_fopen(filename, OPEN_FLAG_READ, fd, revised_path);
	if(err != osd_file::error::NONE) {
		seterror(IMAGE_ERROR_INVALIDIMAGE, "Unable to open the image file");
		return nullptr;
	}

	io_generic io;
	io.file = fd.get();
	io.procs = &corefile_ioprocs_noclose;
	io.filler = 0xff;
	int best = 0;
	floppy_image_format_t *best_format = nullptr;
	for (floppy_image_format_t *format = fif_list; format; format = format->next)
	{
		int score = format->identify(&io, form_factor);
		if(score > best) {
			best = score;
			best_format = format;
		}
	}
	fd.reset();
	return best_format;
}

image_init_result floppy_image_device::call_load()
{
	io_generic io;

	// Do _not_ remove this cast otherwise the pointer will be incorrect when used by the ioprocs.
	io.file = (device_image_interface *)this;
	io.procs = &image_ioprocs;
	io.filler = 0xff;
	int best = 0;
	floppy_image_format_t *best_format = nullptr;
	for(floppy_image_format_t *format = fif_list; format; format = format->next) {
		int score = format->identify(&io, form_factor);
		if(score > best) {
			best = score;
			best_format = format;
		}
	}

	if(!best_format)
	{
		seterror(IMAGE_ERROR_INVALIDIMAGE, "Unable to identify the image format");
		return image_init_result::FAIL;
	}

	image = global_alloc(floppy_image(tracks, sides, form_factor));
	if (!best_format->load(&io, form_factor, image))
	{
		seterror(IMAGE_ERROR_UNSUPPORTED, "Incompatible image format or corrupted data");
		global_free(image);
		image = nullptr;
		return image_init_result::FAIL;
	}
	output_format = is_readonly() ? nullptr : best_format;

	revolution_start_time = mon ? attotime::never : machine().time();
	revolution_count = 0;

	index_resync();
	image_dirty = false;

	wpt = 1; // disk sleeve is covering the sensor
	if (!cur_wpt_cb.isnull())
		cur_wpt_cb(this, wpt);

	wpt = is_readonly() || (output_format == nullptr);
	if (!cur_wpt_cb.isnull())
		cur_wpt_cb(this, wpt);

	if (!cur_load_cb.isnull())
		return cur_load_cb(this);

	if (motor_always_on) {
		// When disk is inserted, start motor
		mon_w(0);
	} else if(!mon)
		ready_counter = 2;

	return image_init_result::PASS;
}

void floppy_image_device::call_unload()
{
	dskchg = 0;

	if (image) {
		if(image_dirty)
			commit_image();
		global_free(image);
		image = nullptr;
	}

	wpt = 1; // disk sleeve is covering the sensor
	if (!cur_wpt_cb.isnull())
		cur_wpt_cb(this, wpt);

	wpt = 0; // sensor is uncovered
	if (!cur_wpt_cb.isnull())
		cur_wpt_cb(this, wpt);

	if (!cur_unload_cb.isnull())
		cur_unload_cb(this);

	if (motor_always_on) {
		// When disk is removed, stop motor
		mon_w(1);
	} else if(!ready) {
		ready = true;
		if(!cur_ready_cb.isnull())
			cur_ready_cb(this, ready);
	}
}

image_init_result floppy_image_device::call_create(int format_type, util::option_resolution *format_options)
{
	image = global_alloc(floppy_image(tracks, sides, form_factor));
	output_format = nullptr;

	// search for a suitable format based on the extension
	for(floppy_image_format_t *i = fif_list; i; i = i->next)
	{
		// only consider formats that actually support saving
		if(!i->supports_save())
			continue;

		if (i->extension_matches(basename()))
		{
			output_format = i;
			break;
		}
	}

	// did we find a suitable format?
	if (output_format == nullptr)
	{
		seterror(IMAGE_ERROR_INVALIDIMAGE, "Unable to identify the image format");
		return image_init_result::FAIL;
	}

	if (motor_always_on)
		// When disk is inserted, start motor
		mon_w(0);

	return image_init_result::PASS;
}

/* motor on, active low */
void floppy_image_device::mon_w(int state)
{
	if(mon == state)
		return;

	mon = state;

	/* off -> on */
	if (!mon && image)
	{
		revolution_start_time = machine().time();
		if (motor_always_on) {
			// Drives with motor that is always spinning are immediately ready when a disk is loaded
			// because there is no spin-up time
			ready = false;
			if(!cur_ready_cb.isnull())
				cur_ready_cb(this, ready);
		} else {
			ready_counter = 2;
		}
		index_resync();
	}

	/* on -> off */
	else {
		if(image_dirty)
			commit_image();
		revolution_start_time = attotime::never;
		index_timer->adjust(attotime::zero);
		if(!ready) {
			ready = true;
			if(!cur_ready_cb.isnull())
				cur_ready_cb(this, ready);
		}
	}

	// Create a motor sound (loaded or empty)
	if (m_make_sound) m_sound_out->motor(state==0, exists());
}

attotime floppy_image_device::time_next_index()
{
	if(revolution_start_time.is_never())
		return attotime::never;
	return revolution_start_time + rev_time;
}

/* index pulses at rpm/60 Hz, and stays high for ~2ms at 300rpm */
void floppy_image_device::index_resync()
{
	if(revolution_start_time.is_never()) {
		if(idx) {
			idx = 0;
			if (!cur_index_pulse_cb.isnull())
				cur_index_pulse_cb(this, idx);
		}
		return;
	}

	attotime delta = machine().time() - revolution_start_time;
	while(delta >= rev_time) {
		delta -= rev_time;
		revolution_start_time += rev_time;
		revolution_count++;
	}
	int position = (delta*floppy_ratio_1).as_ticks(1000000000/1000);

	int new_idx = position < 20000;

	if(new_idx) {
		attotime index_up_time = attotime::from_nsec((2000000*1000)/floppy_ratio_1);
		index_timer->adjust(index_up_time - delta);
	} else
		index_timer->adjust(rev_time - delta);

	if(new_idx != idx) {
		idx = new_idx;
		if(idx && ready) {
			ready_counter--;
			if(!ready_counter) {
				ready = false;
				if(!cur_ready_cb.isnull())
					cur_ready_cb(this, ready);
			}
		}
		if (!cur_index_pulse_cb.isnull())
			cur_index_pulse_cb(this, idx);
	}
}

bool floppy_image_device::ready_r()
{
	return ready;
}

double floppy_image_device::get_pos()
{
	return index_timer->elapsed().as_double();
}

bool floppy_image_device::twosid_r()
{
	int tracks = 0, heads = 0;

	if (image) image->get_actual_geometry(tracks, heads);

	return heads == 1;
}

void floppy_image_device::stp_w(int state)
{
	if ( stp != state ) {
		stp = state;
		if ( stp == 0 ) {
			int ocyl = cyl;
			if ( dir ) {
				if ( cyl ) cyl--;
			} else {
				if ( cyl < tracks-1 ) cyl++;
			}
			if(ocyl != cyl)
			{
				if (TRACE_STEP) logerror("%s: track %d\n", tag(), cyl);
				// Do we want a stepper sound?
				// We plan for 5 zones with possibly specific sounds
				if (m_make_sound) m_sound_out->step(cyl*5/tracks);
			}
			/* Update disk detection if applicable */
			if (exists())
			{
				if (dskchg==0) dskchg = 1;
			}
		}
		subcyl = 0;
	}
}

void floppy_image_device::seek_phase_w(int phases)
{
	int cur_pos = (cyl << 2) | subcyl;
	int req_pos;
	switch(phases) {
	case 0x1: req_pos = 0; break;
	case 0x3: req_pos = 1; break;
	case 0x2: req_pos = 2; break;
	case 0x6: req_pos = 3; break;
	case 0x4: req_pos = 4; break;
	case 0xc: req_pos = 5; break;
	case 0x8: req_pos = 6; break;
	case 0x9: req_pos = 7; break;
	default: return;
	}

	// Opposite phase, don't move
	if(((cur_pos ^ req_pos) & 7) == 4)
		return;

	int next_pos = (cur_pos & ~7) | req_pos;
	if(next_pos < cur_pos-4)
		next_pos += 8;
	else if(next_pos > cur_pos+4)
		next_pos -= 8;
	if(next_pos < 0)
		next_pos = 0;
	else if(next_pos > (tracks-1)*4)
		next_pos = (tracks-1)*4;
	cyl = next_pos >> 2;
	subcyl = next_pos & 3;

	if(TRACE_STEP && (next_pos != cur_pos))
		logerror("%s: track %d.%d\n", tag(), cyl, subcyl);

	/* Update disk detection if applicable */
	if (exists())
		if (dskchg==0)
			dskchg = 1;
}

int floppy_image_device::find_index(uint32_t position, const std::vector<uint32_t> &buf)
{
	int spos = (buf.size() >> 1)-1;
	int step;
	for(step=1; step<buf.size()+1; step<<=1) { }
	step >>= 1;

	for(;;) {
		if(spos >= int(buf.size()) || (spos > 0 && (buf[spos] & floppy_image::TIME_MASK) > position)) {
			spos -= step;
			step >>= 1;
		} else if(spos < 0 || (spos < int(buf.size())-1 && (buf[spos+1] & floppy_image::TIME_MASK) <= position)) {
			spos += step;
			step >>= 1;
		} else
			return spos;
	}
}

uint32_t floppy_image_device::find_position(attotime &base, const attotime &when)
{
	base = revolution_start_time;
	attotime delta = when - base;

	while(delta >= rev_time) {
		delta -= rev_time;
		base += rev_time;
	}
	while(delta < attotime::zero) {
		delta += rev_time;
		base -= rev_time;
	}

	return (delta*floppy_ratio_1).as_ticks(1000000000/1000);
}

attotime floppy_image_device::get_next_index_time(std::vector<uint32_t> &buf, int index, int delta, attotime base)
{
	uint32_t next_position;
	int cells = buf.size();
	if(index+delta < cells)
		next_position = buf[index+delta] & floppy_image::TIME_MASK;
	else {
		if((buf[cells-1]^buf[0]) & floppy_image::MG_MASK)
			delta--;
		index = index + delta - cells + 1;
		next_position = 200000000 + (buf[index] & floppy_image::TIME_MASK);
	}

	return base + attotime::from_nsec((uint64_t(next_position)*2000/floppy_ratio_1+1)/2);
}

attotime floppy_image_device::get_next_transition(const attotime &from_when)
{
	if(!image || mon)
		return attotime::never;

	std::vector<uint32_t> &buf = image->get_buffer(cyl, ss, subcyl);
	uint32_t cells = buf.size();
	if(cells <= 1)
		return attotime::never;

	attotime base;
	uint32_t position = find_position(base, from_when);

	int index = find_index(position, buf);

	if(index == -1)
		return attotime::never;

	attotime result = get_next_index_time(buf, index, 1,  base);
	if(result > from_when)
		return result;
	return get_next_index_time(buf, index, 2,  base);
}

void floppy_image_device::write_flux(const attotime &start, const attotime &end, int transition_count, const attotime *transitions)
{
	if(!image || mon)
		return;
	image_dirty = true;

	attotime base;
	int start_pos = find_position(base, start);
	int end_pos   = find_position(base, end);

	std::vector<int> trans_pos(transition_count);
	for(int i=0; i != transition_count; i++)
		trans_pos[i] = find_position(base, transitions[i]);

	std::vector<uint32_t> &buf = image->get_buffer(cyl, ss, subcyl);

	int index;
	if(!buf.empty())
		index = find_index(start_pos, buf);
	else {
		index = 0;
		buf.push_back(floppy_image::MG_N);
	}

	if(index && (buf[index] & floppy_image::TIME_MASK) == start_pos)
		index--;

	uint32_t cur_mg = buf[index] & floppy_image::MG_MASK;
	if(cur_mg == floppy_image::MG_N || cur_mg == floppy_image::MG_D)
		cur_mg = floppy_image::MG_A;

	uint32_t pos = start_pos;
	int ti = 0;
	int cells = buf.size();
	while(pos != end_pos) {
		if(buf.size() < cells+10)
			buf.resize(cells+200);
		uint32_t next_pos;
		if(ti != transition_count)
			next_pos = trans_pos[ti++];
		else
			next_pos = end_pos;
		if(next_pos > pos)
			write_zone(&buf[0], cells, index, pos, next_pos, cur_mg);
		else {
			write_zone(&buf[0], cells, index, pos, 200000000, cur_mg);
			index = 0;
			write_zone(&buf[0], cells, index, 0, next_pos, cur_mg);
		}
		pos = next_pos;
		cur_mg = cur_mg == floppy_image::MG_A ? floppy_image::MG_B : floppy_image::MG_A;
	}

	buf.resize(cells);
}

void floppy_image_device::write_zone(uint32_t *buf, int &cells, int &index, uint32_t spos, uint32_t epos, uint32_t mg)
{
	while(spos < epos) {
		while(index != cells-1 && (buf[index+1] & floppy_image::TIME_MASK) <= spos)
			index++;

		uint32_t ref_start = buf[index] & floppy_image::TIME_MASK;
		uint32_t ref_end   = index == cells-1 ? 200000000 : buf[index+1] & floppy_image::TIME_MASK;
		uint32_t ref_mg    = buf[index] & floppy_image::MG_MASK;

		// Can't overwrite a damaged zone
		if(ref_mg == floppy_image::MG_D) {
			spos = ref_end;
			continue;
		}

		// If the zone is of the type we want, we don't need to touch it
		if(ref_mg == mg) {
			spos = ref_end;
			continue;
		}

		//  Check the overlaps, act accordingly
		if(spos == ref_start) {
			if(epos >= ref_end) {
				// Full overlap, that cell is dead, we need to see which ones we can extend
				uint32_t prev_mg = index != 0       ? buf[index-1] & floppy_image::MG_MASK : ~0;
				uint32_t next_mg = index != cells-1 ? buf[index+1] & floppy_image::MG_MASK : ~0;
				if(prev_mg == mg) {
					if(next_mg == mg) {
						// Both match, merge all three in one
						memmove(buf+index, buf+index+2, (cells-index-2)*sizeof(uint32_t));
						cells -= 2;
						index--;

					} else {
						// Previous matches, drop the current cell
						memmove(buf+index, buf+index+1, (cells-index-1)*sizeof(uint32_t));
						cells --;
					}

				} else {
					if(next_mg == mg) {
						// Following matches, extend it
						memmove(buf+index, buf+index+1, (cells-index-1)*sizeof(uint32_t));
						cells --;
						buf[index] = mg | spos;
					} else {
						// None match, convert the current cell
						buf[index] = mg | spos;
						index++;
					}
				}
				spos = ref_end;

			} else {
				// Overlap at the start only
				// Check if we can just extend the previous cell
				if(index != 0 && (buf[index-1] & floppy_image::MG_MASK) == mg)
					buf[index] = ref_mg | epos;
				else {
					// Otherwise we need to insert a new cell
					if(index != cells-1)
						memmove(buf+index+1, buf+index, (cells-index)*sizeof(uint32_t));
					cells++;
					buf[index] = mg | spos;
					buf[index+1] = ref_mg | epos;
				}
				spos = epos;
			}

		} else {
			if(epos >= ref_end) {
				// Overlap at the end only
				// If we can't just extend the following cell, we need to insert a new one
				if(index == cells-1 || (buf[index+1] & floppy_image::MG_MASK) != mg) {
					if(index != cells-1)
						memmove(buf+index+2, buf+index+1, (cells-index-1)*sizeof(uint32_t));
					cells++;
				}
				buf[index+1] = mg | spos;
				index++;
				spos = ref_end;

			} else {
				// Full inclusion
				// We need to split the zone in 3
				if(index != cells-1)
					memmove(buf+index+3, buf+index+1, (cells-index-1)*sizeof(uint32_t));
				cells += 2;
				buf[index+1] = mg | spos;
				buf[index+2] = ref_mg | epos;
				spos = epos;
			}
		}

	}
}

void floppy_image_device::set_write_splice(const attotime &when)
{
	if(image) {
		image_dirty = true;
		attotime base;
		int splice_pos = find_position(base, when);
		image->set_write_splice_position(cyl, ss, splice_pos, subcyl);
	}
}

uint32_t floppy_image_device::get_form_factor() const
{
	return form_factor;
}

uint32_t floppy_image_device::get_variant() const
{
	return image ? image->get_variant() : 0;
}

//===================================================================
//   Floppy sound
//
//   In order to enable floppy sound you must add the line
//      MCFG_FLOPPY_DRIVE_SOUND(true)
//   after MCFG_FLOPPY_DRIVE_ADD
//   and you must put audio samples (44100Hz, mono) with names as
//   shown in floppy_sample_names into the directory samples/floppy
//   Sound will be disabled when these samples are missing.
//
//   MZ, Aug 2015
//===================================================================

enum
{
	QUIET=-1,
	SPIN_START_EMPTY=0,
	SPIN_START_LOADED,
	SPIN_EMPTY,
	SPIN_LOADED,
	SPIN_END
};

enum
{
	STEP_SINGLE=0,
	STEP_SEEK2,
	STEP_SEEK6,
	STEP_SEEK12,
	STEP_SEEK20
};

/*
    Unless labeled "constructed", all samples were recorded from real floppy drives.
    The 3.5" floppy drive is a Sony MPF420-1.
    The 5.25" floppy drive is a Chinon FZ502.
*/
static const char *const floppy35_sample_names[] =
{
// Subdirectory
	"*floppy",
// Spinning sounds
	"35_spin_start_empty",
	"35_spin_start_loaded",
	"35_spin_empty",
	"35_spin_loaded",
	"35_spin_end",
// Stepping sounds
	"35_step_1_1",
// Seeking sounds
	"35_seek_2ms",      // constructed
	"35_seek_6ms",
	"35_seek_12ms",
	"35_seek_20ms",
	nullptr
};

static const char *const floppy525_sample_names[] =
{
// Subdirectory
	"*floppy",
// Spinning sounds
	"525_spin_start_empty",
	"525_spin_start_loaded",
	"525_spin_empty",
	"525_spin_loaded",
	"525_spin_end",
// Stepping sounds
	"525_step_1_1",
// Seeking sounds
	"525_seek_2ms",    // unrealistically fast, but needed for 3.5 (constructed)
	"525_seek_6ms",
	"525_seek_12ms",
	"525_seek_20ms",
	nullptr
};

floppy_sound_device::floppy_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: samples_device(mconfig, FLOPPYSOUND, "Floppy sound", tag, owner, clock, "flopsnd", __FILE__),
		m_sound(nullptr),
		m_step_base(0),
		m_spin_samples(0),
		m_step_samples(0),
		m_spin_samplepos(0),
		m_step_samplepos(0),
		m_seek_sound_timeout(0),
		m_zones(0),
		m_spin_playback_sample(QUIET),
		m_step_playback_sample(QUIET),
		m_seek_playback_sample(QUIET),
		m_motor_on(false),
		m_with_disk(false),
		m_loaded(false),
		m_seek_pitch(1.0),
		m_seek_samplepos(0.0)
{
}

void floppy_sound_device::register_for_save_states()
{
	save_item(NAME(m_step_base));
	save_item(NAME(m_spin_samples));
	save_item(NAME(m_step_samples));
	save_item(NAME(m_spin_samplepos));
	save_item(NAME(m_step_samplepos));
	save_item(NAME(m_seek_samplepos));
	save_item(NAME(m_seek_sound_timeout));
	save_item(NAME(m_zones));
	save_item(NAME(m_spin_playback_sample));
	save_item(NAME(m_step_playback_sample));
	save_item(NAME(m_seek_playback_sample));
	save_item(NAME(m_motor_on));
	save_item(NAME(m_with_disk));
	save_item(NAME(m_loaded));
	save_item(NAME(m_seek_pitch));
}

void floppy_sound_device::device_start()
{
	// What kind of drive do we have?
	bool is525 = strstr(tag(), "525") != nullptr;
	static_set_samples_names(*this, is525? floppy525_sample_names : floppy35_sample_names);

	m_motor_on = false;

	// Offsets in the sample collection
	m_spin_samples = 5;
	m_step_base = 5;
	m_step_samples = 1;
	m_zones = 1;             // > 1 needs more than one step sample

	m_spin_samplepos = m_step_samplepos = m_seek_samplepos = 0;
	m_spin_playback_sample = m_step_playback_sample = QUIET;

	// Read audio samples. The samples are stored in the list m_samples.
	m_loaded = load_samples();

	// If we don't have all samples, don't allocate a stream or access sample data.
	if (m_loaded)
	{
		m_sound = machine().sound().stream_alloc(*this, 0, 1, clock()); // per-floppy stream
	}
	register_for_save_states();
}

/*
    Motor sound. Select appropriate sound sample, depending on whether the
    motor is started or keeps running. Motor samples are always fully
    played.
*/
void floppy_sound_device::motor(bool running, bool withdisk)
{
	if (samples_loaded())
	{
		m_sound->update(); // required

		if ((m_spin_playback_sample==QUIET || m_spin_playback_sample==SPIN_END) && running) // motor was either off or already spinning down
		{
			m_spin_samplepos = 0;
			m_spin_playback_sample = withdisk? SPIN_START_LOADED : SPIN_START_EMPTY; // (re)start the motor sound
		}
		else
		{
			// Motor has been running and is turned off now
			if ((m_spin_playback_sample == SPIN_EMPTY || m_spin_playback_sample == SPIN_LOADED) && !running)
				m_spin_playback_sample = SPIN_END; // go to spin down sound when loop is finished
		}
	}
	m_motor_on = running;
	m_with_disk = withdisk;
}

/*
    Activate the step sound.
    The zone parameter should be used to select specific samples for the
    current head position (if available). Its value should range from 0 to 4.
*/
void floppy_sound_device::step(int zone)
{
	if (samples_loaded())
	{
		m_sound->update();  // required

		// Pick one of the step samples
		// TODO: This is only preliminary, need to complete that.
		if (zone >= m_zones) zone = m_zones-1;
		m_step_playback_sample = (zone * m_step_samples) + (machine().rand() % m_step_samples);

		if (m_step_samplepos > 0)
		{
			if (m_seek_playback_sample == QUIET)
			{
				// The last step sample was not completed;
				// we need to find out the step rate
				// With a sample rate of 44100 Hz we can calculate the
				// rate from the sample position
				// 2ms = 88
				// 6ms = 265
				// 12ms = 529
				// 20ms = 882

				if (m_step_samplepos < 100)
				{
					// Should only used for 3.5 drives
					m_seek_playback_sample = STEP_SEEK2;
					m_seek_pitch = 1.0;  // don't use a pitch
				}
				else
				{
					if (m_step_samplepos < 400)       // Use this for 8 ms also
					{
						m_seek_playback_sample = STEP_SEEK6;
						m_seek_pitch = 265.0 / m_step_samplepos;
					}
					else
					{
						if (m_step_samplepos < 600)
						{
							m_seek_playback_sample = STEP_SEEK12;
							m_seek_pitch = 529.0 / m_step_samplepos;
						}
						else
						{
							if (m_step_samplepos < 1200)
							{
								m_seek_playback_sample = STEP_SEEK20;
								m_seek_pitch = 882.0 / m_step_samplepos;
							}
							else
								// For 30ms and longer we replay the step sound
								m_seek_playback_sample = QUIET;
						}
					}
				}
			}

			// Changing the pitch does not always sound convincing
			if (!PITCH_SEEK_SAMPLES) m_seek_pitch = 1;

			if (TRACE_AUDIO) logerror("Seek sample = %d, pitch = %f\n", m_seek_playback_sample, m_seek_pitch);

			// Set the timeout for the seek sound. When it expires,
			// we assume that the seek process is over, and we'll play the
			// rest of the step sound.
			// This will be retriggered with each step pulse.
			m_seek_sound_timeout = m_step_samplepos * 2;
		}
		else
		{
			// Last step sample was completed, this is not a seek process
			m_seek_playback_sample = QUIET;
		}

		// If we switch to the seek sample, let's keep the position of the
		// step sample; else reset the step sample position.
		if (m_seek_playback_sample == QUIET)
			m_step_samplepos = 0;
	}
}

//-------------------------------------------------
//  sound_stream_update - update the sound stream
//-------------------------------------------------

void floppy_sound_device::sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples)
{
	// We are using only one stream, unlike the parent class
	// Also, there is no need for interpolation, as we only expect
	// one sample rate of 44100 for all samples

	int16_t out;
	stream_sample_t *samplebuffer = outputs[0];
	int idx = 0;
	int sampleend = 0;

	while (samples-- > 0)
	{
		out = 0;

		// Motor sound
		if (m_spin_playback_sample != QUIET)
		{
			idx = m_spin_playback_sample;
			sampleend = m_sample[idx].data.size();
			out = m_sample[idx].data[m_spin_samplepos++];

			if (m_spin_samplepos >= sampleend)
			{
				// Motor sample has completed
				switch (m_spin_playback_sample)
				{
				case SPIN_START_EMPTY:
					// After start, switch to the continued spinning sound
					m_spin_playback_sample = SPIN_EMPTY; // move to looping sound
					break;
				case SPIN_START_LOADED:
					// After start, switch to the continued spinning sound
					m_spin_playback_sample = SPIN_LOADED; // move to looping sound
					break;
				case SPIN_EMPTY:
					// As long as the motor pin is asserted, restart the sample
					// play the spindown sample
					if (!m_motor_on) m_spin_playback_sample = SPIN_END; // motor was turned off already (during spin-up maybe) -> spin down
					break;
				case SPIN_LOADED:
					if (!m_motor_on) m_spin_playback_sample = SPIN_END; // motor was turned off already (during spin-up maybe) -> spin down
					break;
				case SPIN_END:
					// Spindown sample over, be quiet or restart if the
					// motor has been restarted
					if (m_motor_on)
						m_spin_playback_sample = m_with_disk? SPIN_START_LOADED : SPIN_START_EMPTY;
					else
						m_spin_playback_sample = QUIET;
					break;
				}
				// Restart the selected sample
				m_spin_samplepos = 0;
			}
		}

		// Seek sound
		// As long as we have a seek sound, there is no step sound
		if (m_seek_sound_timeout == 1)
		{
			// Not retriggered; switch back to the last step sound
			m_seek_playback_sample = QUIET;
			m_seek_sound_timeout = 0;
			// Skip 1/100 sec to dampen the loudest pulse
			// yep, a somewhat dirty trick; we don't have to record yet another sample
			m_step_samplepos += 441;
		}

		if (m_seek_playback_sample != QUIET)
		{
			m_seek_sound_timeout--;

			idx = m_step_base + m_seek_playback_sample;
			sampleend = m_sample[idx].data.size();
			// Mix it into the stream value
			out += m_sample[idx].data[(int)m_seek_samplepos];
			// By adding different values than 1, we can change the playback speed
			// This will be used to adjust the seek sound
			m_seek_samplepos += m_seek_pitch;

			// The seek sample will be replayed without interrupt
			if (m_seek_samplepos >= sampleend)
				m_seek_samplepos = 0;
		}
		else
		{
			// Stepper sound
			if (m_step_playback_sample != QUIET)
			{
				idx = m_step_base + m_step_playback_sample;
				sampleend = m_sample[idx].data.size();

				// Mix it into the stream value
				out += m_sample[idx].data[m_step_samplepos++];
				if (m_step_samplepos >= sampleend)
				{
					// Step sample done
					m_step_samplepos = 0;
					m_step_playback_sample = QUIET;
				}
			}
		}

		// Write to the stream buffer
		*(samplebuffer++) = out;
	}
}

#define FLOPSPK "flopsndout"

MACHINE_CONFIG_FRAGMENT( floppy_img )
	MCFG_SPEAKER_STANDARD_MONO(FLOPSPK)
	MCFG_SOUND_ADD(FLOPSND_TAG, FLOPPYSOUND, 44100)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, FLOPSPK, 0.5)
MACHINE_CONFIG_END

machine_config_constructor floppy_image_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( floppy_img );
}

const device_type FLOPPYSOUND = &device_creator<floppy_sound_device>;


//**************************************************************************
//  GENERIC FLOPPY DRIVE DEFINITIONS
//**************************************************************************

//-------------------------------------------------
//  3" single-sided double density
//-------------------------------------------------

floppy_3_ssdd::floppy_3_ssdd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_3_SSDD, "3\" single sided floppy drive", tag, owner, clock, "floppy_3_ssdd", __FILE__)
{
}

floppy_3_ssdd::~floppy_3_ssdd()
{
}

void floppy_3_ssdd::setup_characteristics()
{
	form_factor = floppy_image::FF_3;
	tracks = 42;
	sides = 1;
	set_rpm(300);
}

void floppy_3_ssdd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSDD;
}

//-------------------------------------------------
//  3" double-sided double density
//-------------------------------------------------

floppy_3_dsdd::floppy_3_dsdd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_3_DSDD, "3\" double sided floppy drive", tag, owner, clock, "floppy_3_dsdd", __FILE__)
{
}

floppy_3_dsdd::~floppy_3_dsdd()
{
}

void floppy_3_dsdd::setup_characteristics()
{
	form_factor = floppy_image::FF_3;
	tracks = 42;
	sides = 2;
	set_rpm(300);
}

void floppy_3_dsdd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::DSDD;
}

//-------------------------------------------------
//  3.5" single-sided double density
//-------------------------------------------------

floppy_35_ssdd::floppy_35_ssdd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_35_SSDD, "3.5\" single-sided double density floppy drive", tag, owner, clock, "floppy_35_ssdd", __FILE__)
{
}

floppy_35_ssdd::~floppy_35_ssdd()
{
}

void floppy_35_ssdd::setup_characteristics()
{
	form_factor = floppy_image::FF_35;
	tracks = 84;
	sides = 1;
	set_rpm(300);
}

void floppy_35_ssdd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
}

//-------------------------------------------------
//  3.5" double-sided double density
//-------------------------------------------------

floppy_35_dd::floppy_35_dd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_35_DD, "3.5\" double density floppy drive", tag, owner, clock, "floppy_35_dd", __FILE__)
{
}

floppy_35_dd::~floppy_35_dd()
{
}

void floppy_35_dd::setup_characteristics()
{
	form_factor = floppy_image::FF_35;
	tracks = 84;
	sides = 2;
	set_rpm(300);
}

void floppy_35_dd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::DSDD;
}

//-------------------------------------------------
//  3.5" high density
//-------------------------------------------------

floppy_35_hd::floppy_35_hd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_35_HD, "3.5\" high density floppy drive", tag, owner, clock, "floppy_35_hd", __FILE__)
{
}

floppy_35_hd::~floppy_35_hd()
{
}

void floppy_35_hd::setup_characteristics()
{
	form_factor = floppy_image::FF_35;
	tracks = 84;
	sides = 2;
	set_rpm(300);
}

void floppy_35_hd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::DSDD;
	variants[var_count++] = floppy_image::DSHD;
}

//-------------------------------------------------
//  3.5" extended density
//-------------------------------------------------

floppy_35_ed::floppy_35_ed(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_35_ED, "3.5\" extended density floppy drive", tag, owner, clock, "floppy_35_ed", __FILE__)
{
}

floppy_35_ed::~floppy_35_ed()
{
}

void floppy_35_ed::setup_characteristics()
{
	form_factor = floppy_image::FF_35;
	tracks = 84;
	sides = 2;
	set_rpm(300);
}

void floppy_35_ed::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::DSDD;
	variants[var_count++] = floppy_image::DSHD;
	variants[var_count++] = floppy_image::DSED;
}

//-------------------------------------------------
//  5.25" single-sided single density 35 tracks
//-------------------------------------------------

floppy_525_sssd_35t::floppy_525_sssd_35t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_525_SSSD_35T, "5.25\" single-sided single density 35-track floppy drive", tag, owner, clock, "floppy_525_sssd_35t", __FILE__)
{
}

floppy_525_sssd_35t::~floppy_525_sssd_35t()
{
}

void floppy_525_sssd_35t::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 35;
	sides = 1;
	set_rpm(300);
}

void floppy_525_sssd_35t::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
}

//-------------------------------------------------
//  5.25" double-sided single density 35 tracks
//-------------------------------------------------

floppy_525_sd_35t::floppy_525_sd_35t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_525_SD_35T, "5.25\" single density 35-track floppy drive", tag, owner, clock, "floppy_525_sd_35t", __FILE__)
{
}

floppy_525_sd_35t::~floppy_525_sd_35t()
{
}

void floppy_525_sd_35t::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 35;
	sides = 2;
	set_rpm(300);
}

void floppy_525_sd_35t::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
}

//-------------------------------------------------
//  5.25" single-sided single density
//-------------------------------------------------

floppy_525_sssd::floppy_525_sssd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_525_SSSD, "5.25\" single-sided single density floppy drive", tag, owner, clock, "floppy_525_sssd", __FILE__)
{
}

floppy_525_sssd::~floppy_525_sssd()
{
}

void floppy_525_sssd::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 42;
	sides = 1;
	set_rpm(300);
}

void floppy_525_sssd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
}

//-------------------------------------------------
//  5.25" double-sided single density
//-------------------------------------------------

floppy_525_sd::floppy_525_sd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_525_SD, "5.25\" single density floppy drive", tag, owner, clock, "floppy_525_sd", __FILE__)
{
}

floppy_525_sd::~floppy_525_sd()
{
}

void floppy_525_sd::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 42;
	sides = 2;
	set_rpm(300);
}

void floppy_525_sd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
}

//-------------------------------------------------
//  5.25" single-sided double density
//-------------------------------------------------

floppy_525_ssdd::floppy_525_ssdd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_525_SSDD, "5.25\" single-sided double density floppy drive", tag, owner, clock, "floppy_525_ssdd", __FILE__)
{
}

floppy_525_ssdd::~floppy_525_ssdd()
{
}

void floppy_525_ssdd::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 42;
	sides = 1;
	set_rpm(300);
}

void floppy_525_ssdd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
}

//-------------------------------------------------
//  5.25" double-sided double density
//-------------------------------------------------

floppy_525_dd::floppy_525_dd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_525_DD, "5.25\" double density floppy drive", tag, owner, clock, "floppy_525_dd", __FILE__)
{
}

floppy_525_dd::~floppy_525_dd()
{
}

void floppy_525_dd::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 42;
	sides = 2;
	set_rpm(300);
}

void floppy_525_dd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::DSDD;
}

//-------------------------------------------------
//  5.25" single-sided quad density
//-------------------------------------------------

floppy_525_ssqd::floppy_525_ssqd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_525_QD, "5.25\" single-sided quad density floppy drive", tag, owner, clock, "floppy_525_ssqd", __FILE__)
{
}

floppy_525_ssqd::~floppy_525_ssqd()
{
}

void floppy_525_ssqd::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 84;
	sides = 1;
	set_rpm(300);
}

void floppy_525_ssqd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::SSQD;
}

//-------------------------------------------------
//  5.25" double-sided quad density
//-------------------------------------------------

floppy_525_qd::floppy_525_qd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_525_QD, "5.25\" quad density floppy drive", tag, owner, clock, "floppy_525_qd", __FILE__)
{
}

floppy_525_qd::~floppy_525_qd()
{
}

void floppy_525_qd::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 84;
	sides = 2;
	set_rpm(300);
}

void floppy_525_qd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::SSQD;
	variants[var_count++] = floppy_image::DSSD;
	variants[var_count++] = floppy_image::DSDD;
	variants[var_count++] = floppy_image::DSQD;
}

//-------------------------------------------------
//  5.25" high density
//-------------------------------------------------

floppy_525_hd::floppy_525_hd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_525_HD, "5.25\" high density floppy drive", tag, owner, clock, "floppy_525_hd", __FILE__)
{
}

floppy_525_hd::~floppy_525_hd()
{
}

void floppy_525_hd::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 84;
	sides = 2;
	set_rpm(360);
}

void floppy_525_hd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::SSQD;
	variants[var_count++] = floppy_image::DSDD;
	variants[var_count++] = floppy_image::DSQD;
	variants[var_count++] = floppy_image::DSHD;
}

//-------------------------------------------------
//  8" single-sided single density
//-------------------------------------------------

floppy_8_sssd::floppy_8_sssd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_8_SSSD, "8\" single density single sided floppy drive", tag, owner, clock, "floppy_8_sssd", __FILE__)
{
}

floppy_8_sssd::~floppy_8_sssd()
{
}

void floppy_8_sssd::setup_characteristics()
{
	form_factor = floppy_image::FF_8;
	tracks = 77;
	sides = 1;
	motor_always_on = true;
	set_rpm(360);
}

void floppy_8_sssd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
}

//-------------------------------------------------
//  8" double-sided single density
//-------------------------------------------------

floppy_8_dssd::floppy_8_dssd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_8_DSSD, "8\" single density double sided floppy drive", tag, owner, clock, "floppy_8_dssd", __FILE__)
{
}

floppy_8_dssd::~floppy_8_dssd()
{
}

void floppy_8_dssd::setup_characteristics()
{
	form_factor = floppy_image::FF_8;
	tracks = 77;
	sides = 2;
	motor_always_on = true;
	set_rpm(360);
}

void floppy_8_dssd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::DSSD;
}

//-------------------------------------------------
//  8" single-sided double density
//-------------------------------------------------

floppy_8_ssdd::floppy_8_ssdd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_8_SSDD, "8\" double density single sided floppy drive", tag, owner, clock, "floppy_8_ssdd", __FILE__)
{
}

floppy_8_ssdd::~floppy_8_ssdd()
{
}

void floppy_8_ssdd::setup_characteristics()
{
	form_factor = floppy_image::FF_8;
	tracks = 77;
	sides = 1;
	motor_always_on = true;
	set_rpm(360);
}

void floppy_8_ssdd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
}

//-------------------------------------------------
//  8" double-sided double density
//-------------------------------------------------

floppy_8_dsdd::floppy_8_dsdd(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, FLOPPY_8_DSDD, "8\" double density double sided floppy drive", tag, owner, clock, "floppy_8_dsdd", __FILE__)
{
}

floppy_8_dsdd::~floppy_8_dsdd()
{
}

void floppy_8_dsdd::setup_characteristics()
{
	form_factor = floppy_image::FF_8;
	tracks = 77;
	sides = 2;
	motor_always_on = true;
	set_rpm(360);
}

void floppy_8_dsdd::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::DSDD;
}


//**************************************************************************
//  SPECIFIC FLOPPY DRIVE DEFINITIONS
//**************************************************************************

//-------------------------------------------------
//  epson smd-165
//
//  track to track: 6 ms
//  average: 97 ms
//  setting time: 15 ms
//  motor start time: 1 s
//
//-------------------------------------------------

epson_smd_165::epson_smd_165(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, EPSON_SD_321, "EPSON SMD-165 Floppy Disk Drive", tag, owner, clock, "epson_smd_165", __FILE__)
{
}

epson_smd_165::~epson_smd_165()
{
}

void epson_smd_165::setup_characteristics()
{
	form_factor = floppy_image::FF_35;
	tracks = 40;
	sides = 2;
	set_rpm(300);
}

void epson_smd_165::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::DSSD;
}

//-------------------------------------------------
//  epson sd-320
//
//  track to track: 15 ms
//  average: 220 ms
//  setting time: 15 ms
//  head load time: 35 ms
//  motor start time: 0.5 s
//
//  dip switch ss1
//  1 = drive select 0
//  2 = drive select 1
//  3 = drive select 2
//  4 = drive select 3
//  5 = head load from pin 4
//  6 = head load from drive select
//
//  dic switch ss2
//  hs = load controlled by head-load
//  ms = load controlled by motor enable
//
//  dic switch ss3
//  ds = in-use led by drive select
//  hl = in-use led by head load
//
//-------------------------------------------------

epson_sd_320::epson_sd_320(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, EPSON_SD_320, "EPSON SD-320 Mini-Floppy Disk Drive", tag, owner, clock, "epson_sd_320", __FILE__)
{
}

epson_sd_320::~epson_sd_320()
{
}

void epson_sd_320::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 40;
	sides = 2;
	set_rpm(300);
}

void epson_sd_320::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::DSDD;
}

//-------------------------------------------------
//  epson sd-321
//
//  same as sd-320, but no head-load selenoid
//
//-------------------------------------------------

epson_sd_321::epson_sd_321(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, EPSON_SD_321, "EPSON SD-321 Mini-Floppy Disk Drive", tag, owner, clock, "epson_sd_321", __FILE__)
{
}

epson_sd_321::~epson_sd_321()
{
}

void epson_sd_321::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 40;
	sides = 2;
	set_rpm(300);
}

void epson_sd_321::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::DSDD;
}

//-------------------------------------------------
//  Sony OA-D31V
//
//  track to track: 15 ms
//  average: 365 ms
//  setting time: 15 ms
//  head load time: 60 ms
//
//-------------------------------------------------

sony_oa_d31v::sony_oa_d31v(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, SONY_OA_D31V, "Sony OA-D31V Micro Floppydisk Drive", tag, owner, clock, "sony_oa_d31v", __FILE__)
{
}

sony_oa_d31v::~sony_oa_d31v()
{
}

void sony_oa_d31v::setup_characteristics()
{
	form_factor = floppy_image::FF_35;
	tracks = 70;
	sides = 1;
	set_rpm(600);
}

void sony_oa_d31v::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
}

//-------------------------------------------------
//  Sony OA-D32W
//
//  track to track: 12 ms
//  average: 350 ms
//  setting time: 30 ms
//  head load time: 60 ms
//  average latency: 50 ms
//
//-------------------------------------------------

sony_oa_d32w::sony_oa_d32w(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, SONY_OA_D32W, "Sony OA-D32W Micro Floppydisk Drive", tag, owner, clock, "sony_oa_d32w", __FILE__)
{
}

sony_oa_d32w::~sony_oa_d32w()
{
}

void sony_oa_d32w::setup_characteristics()
{
	form_factor = floppy_image::FF_35;
	tracks = 80;
	sides = 2;
	set_rpm(600);
}

void sony_oa_d32w::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::DSDD;
}

//-------------------------------------------------
//  Sony OA-D32V
//
//  track to track: 12 ms
//  average: 350 ms
//  setting time: 30 ms
//  head load time: 60 ms
//  average latency: 50 ms
//
//-------------------------------------------------

sony_oa_d32v::sony_oa_d32v(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, SONY_OA_D32V, "Sony OA-D32V Micro Floppydisk Drive", tag, owner, clock, "sony_oa_d32v", __FILE__)
{
}

sony_oa_d32v::~sony_oa_d32v()
{
}

void sony_oa_d32v::setup_characteristics()
{
	form_factor = floppy_image::FF_35;
	tracks = 80;
	sides = 1;
	set_rpm(600);
}

void sony_oa_d32v::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
}

//-------------------------------------------------
//  TEAC FD-55E
//
//  track to track: 3 ms
//  average: 94 ms
//  setting time: 15 ms
//  motor start time: 400 ms
//
//-------------------------------------------------

teac_fd_55e::teac_fd_55e(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, TEAC_FD_55F, "TEAC FD-55E FDD", tag, owner, clock, "teac_fd_55e", __FILE__)
{
}

teac_fd_55e::~teac_fd_55e()
{
}

void teac_fd_55e::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 80;
	sides = 1;
	set_rpm(300);
}

void teac_fd_55e::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::SSQD;
}

//-------------------------------------------------
//  TEAC FD-55F
//
//  track to track: 3 ms
//  average: 94 ms
//  setting time: 15 ms
//  motor start time: 400 ms
//
//-------------------------------------------------

teac_fd_55f::teac_fd_55f(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, TEAC_FD_55F, "TEAC FD-55F FDD", tag, owner, clock, "teac_fd_55f", __FILE__)
{
}

teac_fd_55f::~teac_fd_55f()
{
}

void teac_fd_55f::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 80;
	sides = 2;
	set_rpm(300);
}

void teac_fd_55f::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::SSQD;
	variants[var_count++] = floppy_image::DSSD;
	variants[var_count++] = floppy_image::DSDD;
	variants[var_count++] = floppy_image::DSQD;
}

//-------------------------------------------------
//  TEAC FD-55G
//
//  track to track: 3 ms
//  average: 91 ms
//  setting time: 15 ms
//  motor start time: 400 ms
//
//-------------------------------------------------

teac_fd_55g::teac_fd_55g(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, TEAC_FD_55G, "TEAC FD-55G FDD", tag, owner, clock, "teac_fd_55g", __FILE__)
{
}

teac_fd_55g::~teac_fd_55g()
{
}

void teac_fd_55g::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 77;
	sides = 2;
	set_rpm(360);
}

void teac_fd_55g::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
	variants[var_count++] = floppy_image::SSDD;
	variants[var_count++] = floppy_image::SSQD;
	variants[var_count++] = floppy_image::DSDD;
	variants[var_count++] = floppy_image::DSQD;
	variants[var_count++] = floppy_image::DSHD;
}

//-------------------------------------------------
//  ALPS 32551901 (black) / 32551902 (brown)
//
//  used in the Commodoere 1541 disk drive
//-------------------------------------------------

alps_3255190x::alps_3255190x(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	floppy_image_device(mconfig, ALPS_3255190x, "ALPS 32551901/32551902 Floppy Drive", tag, owner, clock, "alps_3255190x", __FILE__)
{
}

alps_3255190x::~alps_3255190x()
{
}

void alps_3255190x::setup_characteristics()
{
	form_factor = floppy_image::FF_525;
	tracks = 84;
	sides = 1;
	set_rpm(300);
	cyl = 34;
}

void alps_3255190x::handled_variants(uint32_t *variants, int &var_count) const
{
	var_count = 0;
	variants[var_count++] = floppy_image::SSSD;
}
