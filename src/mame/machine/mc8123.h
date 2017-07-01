// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria, David Widel
/***************************************************************************

  NEC MC-8123 encryption emulation

***************************************************************************/

// this function assumes a fixed portion of ROM at 0000-7FFF, and
// an arbitrary amount of banks at 8000-BFFF.

void mc8123_decode(uint8_t *rom, uint8_t *opcodes, const uint8_t *key, int length);
