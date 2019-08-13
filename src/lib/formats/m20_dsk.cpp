// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/m20_dsk.c

    Olivetti M20 floppy-disk images

    Track 0/head 0 is FM, 128 byte sectors. The rest is MFM,
    256 byte sectors.
    In image files the sectors of track 0/sector 0 are 256 bytes
    long to simplify access. Only the first half of these sectors
    contain image data.

*********************************************************************/

#include "m20_dsk.h"

m20_format::m20_format()
{
}

const char *m20_format::name() const
{
	return "m20";
}

const char *m20_format::description() const
{
	return "M20 disk image";
}

const char *m20_format::extensions() const
{
	return "img";
}

bool m20_format::supports_save() const
{
	return true;
}

int m20_format::identify(io_generic *io, uint32_t form_factor)
{
	if(io_generic_size(io) == 286720)
		return 50;
	return 0;
}

bool m20_format::load(io_generic *io, uint32_t form_factor, floppy_image *image)
{
	for (int track = 0; track < 35; track++)
		for (int head = 0; head < 2; head ++) {
			bool mfm = track || head;
			desc_pc_sector sects[16];
			uint8_t sectdata[16*256];
			io_generic_read(io, sectdata, 16*256*(track*2+head), 16*256);
			for (int i = 0; i < 16; i++) {
				int j = i/2 + (i & 1 ? 0 : 8);
				sects[i].track = track;
				sects[i].head = head;
				sects[i].sector = j+1;
				sects[i].size = mfm ? 1 : 0;
				sects[i].actual_size = mfm ? 256 : 128;
				sects[i].data = sectdata + 256*j;
				sects[i].deleted = false;
				sects[i].bad_crc = false;
			}

			if(mfm)
				build_wd_track_mfm(track, head, image, 100000, 16, sects, 50, 32, 22);
			else
				build_wd_track_fm(track, head, image, 50000, 16, sects, 24, 16, 11);
		}

	return true;
}

bool m20_format::save(io_generic *io, floppy_image *image)
{
	uint8_t bitstream[500000/8];
	uint8_t sector_data[50000];
	desc_xs sectors[256];
	int track_size;
	uint64_t file_offset = 0;

	int track_count, head_count;
	track_count = 35; head_count = 2;  //FIXME: use image->get_actual_geometry(track_count, head_count) instead

	// initial fm track
	generate_bitstream_from_track(0, 0, 4000, bitstream, track_size, image);
	extract_sectors_from_bitstream_fm_pc(bitstream, track_size, sectors, sector_data, sizeof(sector_data));

	for (int i = 0; i < 16; i++)
	{
		io_generic_write(io, sectors[i + 1].data, file_offset, 128);
		file_offset += 256; //128;
	}

	// rest are mfm tracks
	for (int track = 0; track < track_count; track++)
	{
		for (int head = 0; head < head_count; head++)
		{
			// skip track 0, head 0
			if (track == 0) { if (head_count == 1) break; else head++; }

			generate_bitstream_from_track(track, head, 2000, bitstream, track_size, image);
			extract_sectors_from_bitstream_mfm_pc(bitstream, track_size, sectors, sector_data, sizeof(sector_data));

			for (int i = 0; i < 16; i++)
			{
				io_generic_write(io, sectors[i + 1].data, file_offset, 256);
				file_offset += 256;
			}
		}
	}

	return true;
}

const floppy_format_type FLOPPY_M20_FORMAT = &floppy_image_format_creator<m20_format>;
