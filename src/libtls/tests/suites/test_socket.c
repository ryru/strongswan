/*
 * Copyright (C) 2020 Pascal Knecht
 * HSR Hochschule fuer Technik Rapperswil
 *
 * Copyright (C) 2014 Martin Willi
 * Copyright (C) 2014 revosec AG
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include <test_suite.h>

#include <unistd.h>
#include <errno.h>

#include <processing/jobs/callback_job.h>
#include <credentials/sets/mem_cred.h>

#include "tls_socket.h"

/**
 * Credentials for authentication
 */
static mem_cred_t *creds;

/**
 * RSA private key, 2048 bit
 */
static char rsa[] = {
	0x30,0x82,0x04,0xa4,0x02,0x01,0x00,0x02,0x82,0x01,0x01,0x00,0xee,0xa3,0x28,0xcc,
	0x48,0xca,0x37,0xfc,0xb6,0xfa,0xfc,0x18,0x0d,0xa2,0x28,0x44,0xb4,0x16,0x56,0xf7,
	0x97,0x5f,0x38,0x83,0xfc,0xd4,0x30,0xea,0xf7,0x5e,0xaa,0xd4,0x21,0x0e,0x71,0x49,
	0x13,0x39,0xaf,0x89,0xa1,0x1d,0x1b,0x9a,0x08,0x44,0xff,0x0b,0xeb,0x4b,0xad,0x8e,
	0xc4,0x6d,0x1e,0x0c,0x02,0xbb,0x17,0x59,0xc7,0x66,0xc7,0xff,0x4c,0x3c,0x11,0x40,
	0x1a,0xe3,0xca,0x34,0xf8,0x41,0xe0,0x39,0x3e,0xce,0x72,0x9f,0x56,0x9e,0x69,0xad,
	0x98,0x43,0x5f,0x35,0xc2,0xd0,0xd9,0xbc,0x8b,0xed,0xc6,0xc7,0x74,0x73,0x74,0x30,
	0x92,0x86,0x39,0x26,0x3d,0xf1,0xd5,0x16,0x45,0x7d,0xcc,0x90,0x54,0xff,0x44,0x74,
	0xf3,0xba,0x41,0x5c,0x58,0xa4,0x66,0xe6,0x9d,0x58,0xbe,0x7e,0x89,0xe1,0x7c,0xf7,
	0x28,0xb0,0xde,0xe2,0x01,0x0a,0x89,0xc7,0x63,0x3f,0xef,0x2b,0xcb,0xef,0x65,0x89,
	0x82,0x23,0x32,0xa7,0xa3,0x1c,0x0d,0xc6,0x8f,0x76,0x59,0x8b,0x55,0x65,0x9c,0x91,
	0xd4,0x93,0x89,0xad,0x37,0x47,0x23,0x25,0xb3,0x53,0xea,0xef,0x73,0xeb,0x97,0xd3,
	0xd7,0x74,0x38,0x73,0x8d,0x16,0x0d,0x6f,0xae,0x59,0x33,0x4e,0x24,0xe9,0x52,0xf6,
	0x6f,0x8c,0x5c,0x13,0xcf,0x1d,0x0a,0xcc,0xb7,0x6a,0x88,0xce,0x91,0xe2,0xe0,0xcb,
	0xc6,0xd2,0xfb,0x81,0xf6,0xd2,0x9f,0x0a,0x82,0x70,0x80,0xbf,0x93,0x70,0xc0,0x57,
	0x23,0x6e,0x97,0x1c,0x9d,0x7d,0xf0,0xa3,0x54,0x86,0xec,0x40,0xae,0x09,0x20,0xed,
	0x02,0x43,0xa3,0xf8,0x7e,0x0e,0x5b,0xd0,0x22,0x7b,0x74,0x39,0x02,0x03,0x01,0x00,
	0x01,0x02,0x82,0x01,0x01,0x00,0xd9,0x5b,0x99,0x74,0x80,0xb4,0x57,0xcc,0x82,0x2a,
	0x17,0x66,0x1d,0x3c,0xde,0xea,0xbd,0x11,0x40,0x03,0x62,0x47,0xe3,0xe5,0x2c,0x6b,
	0x65,0x67,0x0f,0x0b,0x96,0x13,0x83,0x4c,0x71,0x58,0xfa,0xfe,0xe6,0xe9,0x37,0xeb,
	0x98,0x51,0x73,0x48,0xcc,0xf9,0xe1,0x46,0x5b,0xfe,0x16,0xe1,0xc0,0xa5,0x75,0xf3,
	0x4d,0x30,0x84,0x14,0x15,0x04,0x6f,0x3e,0xa3,0x03,0xbd,0xba,0x4f,0x5a,0x71,0xe9,
	0x26,0xbf,0x5d,0x7a,0x93,0x22,0x98,0xb5,0xcf,0x51,0xc3,0xc7,0x51,0xb8,0x59,0x0a,
	0xfb,0xd7,0xe5,0xa8,0x1d,0x0f,0x5c,0xfd,0x30,0x0e,0x71,0xd7,0x79,0xc4,0x60,0x55,
	0x9e,0x1e,0x1c,0x0b,0x9a,0x40,0xb8,0x7a,0x8d,0xb2,0xec,0xb0,0x70,0x8a,0x19,0x5f,
	0x1d,0x2e,0xde,0x90,0x8f,0x68,0x56,0x08,0xce,0x0c,0x08,0xde,0xc7,0xf8,0x13,0xef,
	0xd2,0xbc,0x92,0xb6,0xfb,0xec,0xb6,0x04,0xf6,0x8f,0x7d,0x95,0xe9,0xeb,0xc7,0xfb,
	0xcc,0x4f,0xad,0x41,0xf1,0x4c,0x79,0x07,0xdd,0x4b,0x40,0xb4,0x74,0x44,0x9a,0x06,
	0x0a,0x0f,0xb2,0xda,0x12,0x46,0xe5,0xee,0x01,0x64,0xe5,0xf0,0x82,0x69,0xf9,0xf1,
	0xe9,0x41,0x13,0x5a,0xee,0xc0,0x37,0x9a,0xbe,0x9a,0x9a,0x06,0x4b,0x52,0xd6,0xf3,
	0x1b,0x30,0x64,0x93,0x3a,0x97,0xe1,0xdc,0x50,0x1f,0x46,0xc4,0x81,0x6a,0x17,0x52,
	0x49,0x85,0xc6,0x85,0xb7,0x60,0xd4,0xf0,0xd1,0x6a,0xeb,0x50,0x8c,0xb7,0xeb,0x1f,
	0x17,0x0e,0xf0,0xfd,0x67,0x03,0x7c,0x74,0x1a,0xac,0x66,0x81,0x00,0x45,0x5e,0xf3,
	0xd9,0x9d,0x22,0x99,0xc4,0x11,0x02,0x81,0x81,0x00,0xfa,0x44,0x32,0x14,0xb2,0x82,
	0x28,0x02,0x46,0x05,0xdd,0x8d,0xb1,0x9f,0x9e,0x6f,0x61,0xf2,0x01,0xa0,0x2b,0x76,
	0xee,0x46,0xaa,0x2d,0x2d,0x5b,0xd2,0x67,0x90,0x36,0xbb,0xa0,0x07,0xdf,0x9b,0xad,
	0x18,0x1e,0xa7,0xe6,0x36,0xc6,0x49,0xda,0xc5,0x0d,0x52,0x29,0x5a,0x40,0xcf,0xdf,
	0x8d,0xd0,0xa3,0xc2,0x34,0x17,0x9f,0xb5,0xf1,0x67,0xac,0x29,0x10,0xc2,0x5c,0x62,
	0xe3,0xe2,0x5c,0x9f,0x93,0xcc,0xb5,0xeb,0x16,0x64,0x44,0x9f,0x6b,0x5a,0xac,0x19,
	0x09,0xff,0x4b,0x78,0x7f,0xec,0x5a,0xbd,0xe9,0xcb,0x74,0xbb,0x30,0x13,0xc5,0x25,
	0x8b,0xac,0x8d,0xf9,0xa9,0x99,0x25,0xf5,0xce,0x07,0xb6,0x2b,0x1b,0x42,0xed,0x3a,
	0x30,0x4a,0xfc,0x5f,0xf0,0xe2,0x26,0xa6,0x60,0x5d,0x02,0x81,0x81,0x00,0xf4,0x1a,
	0xc2,0x7e,0xa0,0xa0,0xad,0x20,0x65,0x04,0xe8,0xf7,0xb0,0xb1,0x76,0x79,0x08,0x18,
	0x58,0x93,0x21,0xf1,0x56,0x58,0x58,0x18,0x4a,0x5c,0x59,0x08,0x27,0x64,0x09,0xcb,
	0x0b,0x0b,0x4e,0x26,0xc8,0x0b,0x87,0x67,0x40,0xc1,0xab,0x31,0x60,0xa6,0x78,0xdd,
	0x78,0xc8,0x86,0x38,0xbd,0x19,0xde,0x0b,0x70,0x72,0xec,0x36,0x88,0x39,0x69,0x70,
	0xda,0xa6,0x2e,0xf9,0x5c,0xd8,0x17,0xc5,0xfa,0xf8,0xa5,0xc9,0x9b,0xf0,0xfe,0x03,
	0x71,0x57,0xfa,0x58,0x0f,0x33,0xc3,0xab,0xce,0xb0,0x5d,0xd0,0x40,0x07,0x9a,0x0b,
	0xff,0xb9,0xaa,0x9d,0xc5,0x33,0x7f,0x5f,0x48,0x7e,0x54,0x82,0xd1,0xdf,0x75,0x69,
	0xee,0xe5,0xf5,0x80,0x44,0xce,0x52,0x72,0x14,0x2c,0xe6,0xa7,0xd5,0x8d,0x02,0x81,
	0x81,0x00,0xb8,0xf7,0x70,0x20,0x35,0xf2,0xd6,0x89,0x1f,0xa1,0xb4,0x26,0xc6,0x51,
	0xd7,0xb2,0x30,0xac,0xc1,0xa0,0xd4,0x9e,0xf8,0xea,0x87,0x5a,0x0e,0x7d,0x1f,0xdb,
	0xe5,0x0d,0x5e,0xcc,0x9f,0x25,0x18,0x14,0xed,0x8f,0xb2,0xbe,0x06,0x5b,0xb5,0x38,
	0x18,0x8d,0x88,0xdd,0x01,0x54,0x87,0x8e,0x8d,0x6c,0xd7,0xab,0x6f,0xfe,0xc9,0xce,
	0x9a,0x15,0xea,0x7b,0x0b,0x64,0xeb,0x0d,0x37,0xaa,0x14,0x94,0xe8,0x92,0xd3,0x1d,
	0x66,0x16,0x43,0x55,0xa3,0xed,0x86,0xe6,0x96,0xa9,0xf5,0xe8,0xa0,0x7b,0x5a,0x71,
	0xa4,0x7a,0xf7,0xd2,0x65,0x6d,0x27,0x37,0x61,0xac,0xed,0xdd,0xc9,0x08,0x64,0xb2,
	0xf0,0x4c,0x68,0xca,0x21,0x42,0xec,0xbc,0x25,0xf7,0x35,0xe1,0xde,0xd1,0xf6,0x88,
	0xdf,0x0d,0x02,0x81,0x80,0x44,0xb0,0xcb,0x0e,0x6b,0x11,0x0b,0xe6,0xd3,0xc6,0x7f,
	0xf0,0x43,0x6e,0x8c,0xd2,0x1e,0x2f,0x0b,0xad,0xcb,0x9d,0x68,0x18,0xd0,0x21,0x75,
	0xbb,0x6a,0xea,0x5a,0x7b,0x52,0x2e,0x2a,0xdb,0x71,0x90,0x84,0x36,0x8a,0x51,0xc9,
	0xed,0x35,0xc9,0x5d,0x53,0x3b,0x2b,0xc7,0x73,0x56,0x21,0xdd,0x44,0xcc,0x31,0x17,
	0xe1,0x9f,0x0a,0xf1,0x66,0x86,0x7f,0x55,0x67,0xf2,0x4c,0x05,0x8e,0x61,0x92,0x3a,
	0xbf,0x81,0x97,0xac,0x24,0x32,0xb6,0xb1,0x4c,0x7a,0x8c,0x11,0x2b,0x15,0xe2,0xe0,
	0xf4,0xcc,0x51,0x6f,0xd3,0x33,0xcc,0x30,0x98,0x04,0xa5,0x04,0xfb,0x2a,0xda,0x9b,
	0x41,0xc1,0x72,0x56,0xb0,0xb5,0x0f,0xac,0x44,0x55,0xc3,0x54,0x99,0x62,0xa5,0xeb,
	0x7b,0x7f,0x24,0xb7,0x79,0x02,0x81,0x80,0x0a,0x3b,0x9b,0x91,0x1d,0x9b,0x04,0x4e,
	0xdf,0xd9,0xe6,0x47,0xf3,0x79,0xb7,0x17,0xcf,0x42,0xa5,0xde,0x94,0xf0,0xfe,0xed,
	0x46,0xf6,0xaf,0x3e,0x6c,0x91,0x01,0x89,0x79,0x81,0xea,0x2b,0x82,0x68,0x0e,0xd8,
	0x25,0xaf,0x79,0x8b,0x14,0xfd,0xf2,0x29,0x20,0x34,0x2d,0x0b,0x08,0x8c,0x3b,0x2b,
	0xfc,0x75,0xe9,0x4e,0x21,0xa6,0xb2,0x35,0x67,0x8d,0x4c,0x90,0x94,0x02,0xd5,0x32,
	0x23,0xc6,0xa0,0x92,0x2e,0xfa,0x97,0x48,0x5b,0x95,0xc3,0xf1,0xbc,0x6b,0xe8,0x4c,
	0x92,0x6f,0x5e,0x3d,0xf9,0xbd,0x2c,0xf0,0x83,0x1c,0xe6,0xb3,0x45,0x68,0x32,0x8d,
	0x85,0x20,0xcb,0x9d,0xd2,0x30,0x5a,0x57,0xa4,0x6e,0x27,0xb5,0x29,0x14,0xdb,0xf1,
	0x4b,0x9a,0xc3,0xc1,0xc5,0x37,0x6d,0x1b,
};

/**
 * ECDSA private key
 */
static char ecdsa[] = {
	0x30,0x81,0xa4,0x02,0x01,0x01,0x04,0x30,0xc0,0x1f,0xfd,0x65,0xc6,0xc4,0x4c,0xb8,
	0xff,0x56,0x08,0xb5,0xbd,0xb8,0xf5,0x93,0xf7,0x51,0x0e,0x92,0x1f,0x06,0xbf,0xa6,
	0xd9,0x1d,0xae,0xa3,0x16,0x0d,0x0f,0xc9,0xd5,0x97,0x90,0x46,0xf1,0x98,0xa8,0x18,
	0x07,0xba,0xcf,0x91,0x8e,0x07,0xed,0x88,0xa0,0x07,0x06,0x05,0x2b,0x81,0x04,0x00,
	0x22,0xa1,0x64,0x03,0x62,0x00,0x04,0xd6,0xba,0xe1,0xf0,0x09,0x22,0x21,0x12,0x69,
	0xed,0x0e,0xd5,0x02,0x8c,0xb8,0x52,0xbb,0x57,0x68,0x0e,0xf3,0xdb,0xb9,0xb1,0xee,
	0x9c,0x67,0xa0,0xb8,0xdc,0x13,0x1e,0x5b,0x44,0x71,0x04,0xef,0x4e,0xe3,0xdd,0xf4,
	0xa6,0xc3,0xba,0x77,0x53,0xb8,0x28,0x5f,0xd2,0x97,0x05,0xa3,0x5b,0xe6,0xde,0x0a,
	0xce,0x11,0xa8,0xaf,0x02,0xbd,0xfa,0x17,0xf9,0xa7,0x38,0x3e,0x5b,0x57,0xb0,0x01,
	0xb3,0xc6,0x09,0x29,0x65,0xae,0xfb,0x87,0x92,0xa3,0xd7,0x3d,0x9a,0x1c,0x52,0x09,
	0xb1,0x47,0xc8,0xf6,0x18,0xbb,0x97,
};

/**
 * Ed25519 private key
 * pki --gen --type ed25519
 */
static char ed25519[] = {
	0x30,0x2e,0x02,0x01,0x00,0x30,0x05,0x06,0x03,0x2b,0x65,0x70,0x04,0x22,0x04,0x20,
	0x70,0x4b,0xca,0x70,0x1c,0xb1,0x75,0xf1,0xed,0xde,0xf1,0x99,0x8e,0x11,0x32,0x2d,
	0x76,0x41,0x01,0x6c,0xd7,0xbc,0x79,0xa2,0x06,0x15,0x7e,0x44,0x28,0xf2,0x49,0x61,
};

/**
 * Ed448 private key
 * pki --gen --type ed448
 */
static char ed448[] = {
	0x30,0x47,0x02,0x01,0x00,0x30,0x05,0x06,0x03,0x2b,0x65,0x71,0x04,0x3b,0x04,0x39,
	0xcf,0xb9,0xbe,0x75,0xdf,0x76,0x8b,0x9a,0xdf,0x56,0x52,0x5d,0x37,0x7d,0xb0,0xa8,
	0x1f,0x19,0x44,0x30,0xbd,0x9d,0x0e,0xff,0x5f,0xc8,0xc3,0xec,0x60,0xd5,0xf6,0xa4,
	0x3d,0x1f,0x0e,0xb5,0x65,0x6e,0xe6,0x62,0x4d,0xb7,0xe7,0x5c,0x88,0x7b,0xe8,0xdd,
	0x02,0xd9,0x0f,0xaa,0xe7,0xfb,0x05,0x14,0x41,
};

/**
 * TLS certificate for RSA key
 * pki --self --in rsa.key --dn "C=CH, O=strongSwan, CN=tls-rsa" --san 127.0.0.1
 */
static char rsa_crt[] = {
	0x30,0x82,0x02,0xfd,0x30,0x82,0x01,0xe5,0xa0,0x03,0x02,0x01,0x02,0x02,0x08,0x78,
	0x90,0x39,0xe2,0x1c,0x03,0x1f,0x05,0x30,0x0d,0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,
	0x0d,0x01,0x01,0x0b,0x05,0x00,0x30,0x34,0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,
	0x06,0x13,0x02,0x43,0x48,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0a,0x13,0x0a,
	0x73,0x74,0x72,0x6f,0x6e,0x67,0x53,0x77,0x61,0x6e,0x31,0x10,0x30,0x0e,0x06,0x03,
	0x55,0x04,0x03,0x13,0x07,0x74,0x6c,0x73,0x2d,0x72,0x73,0x61,0x30,0x1e,0x17,0x0d,
	0x32,0x30,0x30,0x33,0x32,0x35,0x31,0x34,0x32,0x39,0x32,0x37,0x5a,0x17,0x0d,0x32,
	0x33,0x30,0x33,0x32,0x35,0x31,0x34,0x32,0x39,0x32,0x37,0x5a,0x30,0x34,0x31,0x0b,
	0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x43,0x48,0x31,0x13,0x30,0x11,0x06,
	0x03,0x55,0x04,0x0a,0x13,0x0a,0x73,0x74,0x72,0x6f,0x6e,0x67,0x53,0x77,0x61,0x6e,
	0x31,0x10,0x30,0x0e,0x06,0x03,0x55,0x04,0x03,0x13,0x07,0x74,0x6c,0x73,0x2d,0x72,
	0x73,0x61,0x30,0x82,0x01,0x22,0x30,0x0d,0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,
	0x01,0x01,0x01,0x05,0x00,0x03,0x82,0x01,0x0f,0x00,0x30,0x82,0x01,0x0a,0x02,0x82,
	0x01,0x01,0x00,0xee,0xa3,0x28,0xcc,0x48,0xca,0x37,0xfc,0xb6,0xfa,0xfc,0x18,0x0d,
	0xa2,0x28,0x44,0xb4,0x16,0x56,0xf7,0x97,0x5f,0x38,0x83,0xfc,0xd4,0x30,0xea,0xf7,
	0x5e,0xaa,0xd4,0x21,0x0e,0x71,0x49,0x13,0x39,0xaf,0x89,0xa1,0x1d,0x1b,0x9a,0x08,
	0x44,0xff,0x0b,0xeb,0x4b,0xad,0x8e,0xc4,0x6d,0x1e,0x0c,0x02,0xbb,0x17,0x59,0xc7,
	0x66,0xc7,0xff,0x4c,0x3c,0x11,0x40,0x1a,0xe3,0xca,0x34,0xf8,0x41,0xe0,0x39,0x3e,
	0xce,0x72,0x9f,0x56,0x9e,0x69,0xad,0x98,0x43,0x5f,0x35,0xc2,0xd0,0xd9,0xbc,0x8b,
	0xed,0xc6,0xc7,0x74,0x73,0x74,0x30,0x92,0x86,0x39,0x26,0x3d,0xf1,0xd5,0x16,0x45,
	0x7d,0xcc,0x90,0x54,0xff,0x44,0x74,0xf3,0xba,0x41,0x5c,0x58,0xa4,0x66,0xe6,0x9d,
	0x58,0xbe,0x7e,0x89,0xe1,0x7c,0xf7,0x28,0xb0,0xde,0xe2,0x01,0x0a,0x89,0xc7,0x63,
	0x3f,0xef,0x2b,0xcb,0xef,0x65,0x89,0x82,0x23,0x32,0xa7,0xa3,0x1c,0x0d,0xc6,0x8f,
	0x76,0x59,0x8b,0x55,0x65,0x9c,0x91,0xd4,0x93,0x89,0xad,0x37,0x47,0x23,0x25,0xb3,
	0x53,0xea,0xef,0x73,0xeb,0x97,0xd3,0xd7,0x74,0x38,0x73,0x8d,0x16,0x0d,0x6f,0xae,
	0x59,0x33,0x4e,0x24,0xe9,0x52,0xf6,0x6f,0x8c,0x5c,0x13,0xcf,0x1d,0x0a,0xcc,0xb7,
	0x6a,0x88,0xce,0x91,0xe2,0xe0,0xcb,0xc6,0xd2,0xfb,0x81,0xf6,0xd2,0x9f,0x0a,0x82,
	0x70,0x80,0xbf,0x93,0x70,0xc0,0x57,0x23,0x6e,0x97,0x1c,0x9d,0x7d,0xf0,0xa3,0x54,
	0x86,0xec,0x40,0xae,0x09,0x20,0xed,0x02,0x43,0xa3,0xf8,0x7e,0x0e,0x5b,0xd0,0x22,
	0x7b,0x74,0x39,0x02,0x03,0x01,0x00,0x01,0xa3,0x13,0x30,0x11,0x30,0x0f,0x06,0x03,
	0x55,0x1d,0x11,0x04,0x08,0x30,0x06,0x87,0x04,0x7f,0x00,0x00,0x01,0x30,0x0d,0x06,
	0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x0b,0x05,0x00,0x03,0x82,0x01,0x01,
	0x00,0x97,0xfb,0x1e,0xd8,0x78,0x65,0x65,0x23,0x15,0xf9,0x8a,0xcc,0x98,0x37,0x30,
	0x05,0x34,0x76,0x6a,0x23,0x11,0x40,0x89,0x9f,0x26,0x33,0xd5,0x00,0xa9,0xf2,0x78,
	0x42,0x99,0xa7,0xbd,0xda,0x31,0xa1,0xcd,0x56,0x46,0xfe,0xd1,0xa3,0x9c,0x86,0xfa,
	0x0b,0x76,0xb2,0x13,0x8f,0x83,0x96,0x00,0x5a,0xa0,0xc5,0x63,0x72,0xd5,0xf8,0xbe,
	0x61,0x57,0x3e,0xd4,0xaa,0x8c,0x87,0xdf,0x27,0xf3,0x6f,0xbe,0xbe,0x3c,0x50,0xa1,
	0x7a,0x00,0xbd,0x0e,0x16,0x1a,0x1f,0xde,0xd9,0xf7,0x76,0xdd,0xc9,0xa9,0xff,0x82,
	0xf7,0x59,0x03,0x2b,0x6e,0x19,0x7c,0xac,0x6d,0x29,0xe6,0xc8,0x83,0x35,0x91,0x04,
	0xf2,0x6a,0x9b,0x3c,0x54,0xda,0x83,0x72,0x80,0x07,0xc3,0x8e,0x4c,0x1c,0x09,0xbf,
	0x41,0x34,0xec,0xa1,0xac,0xc3,0xac,0x71,0x5d,0x64,0x1e,0x60,0x4b,0x98,0xaa,0x1f,
	0x0c,0x7f,0x6d,0xf2,0xe1,0x56,0x28,0x48,0xf0,0xfd,0xe4,0xb0,0xb6,0xad,0xf3,0x49,
	0x76,0x6b,0x23,0x35,0x73,0x38,0xd6,0x1c,0x6d,0x73,0xd4,0x0c,0x8e,0x1a,0x15,0x75,
	0x90,0x76,0xee,0xe1,0x2d,0x90,0xf6,0x1a,0x49,0xb4,0x19,0xda,0xcd,0x89,0xd6,0x8d,
	0x27,0xb0,0xeb,0x79,0x4e,0x9f,0xec,0xda,0xbf,0x27,0x84,0xf0,0xd5,0x5b,0xef,0x0b,
	0xea,0xec,0x35,0x99,0x9b,0x39,0xe7,0x43,0x24,0xc6,0x71,0x4c,0xf8,0x6f,0x7a,0xc9,
	0x4a,0x2e,0xbb,0x96,0x77,0x05,0xe3,0xb3,0xcf,0x1f,0x80,0xbe,0xd1,0xed,0xde,0x85,
	0xaf,0xb4,0xe3,0x63,0x63,0xdc,0x70,0x4e,0x1e,0x0e,0x9a,0x10,0x2d,0xe5,0x86,0x6e,
	0xbd,
};

/**
 * TLS certificate for ECDSA key
 * pki --self --in ecdsa.key --dn "C=CH, O=strongSwan, CN=tls-ecdsa" --san 127.0.0.1
 */
static char ecdsa_crt[] = {
	0x30,0x82,0x01,0xb1,0x30,0x82,0x01,0x38,0xa0,0x03,0x02,0x01,0x02,0x02,0x08,0x77,
	0x8f,0x61,0x26,0xa2,0xae,0xe8,0x6c,0x30,0x0a,0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,
	0x04,0x03,0x03,0x30,0x36,0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,
	0x43,0x48,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0a,0x13,0x0a,0x73,0x74,0x72,
	0x6f,0x6e,0x67,0x53,0x77,0x61,0x6e,0x31,0x12,0x30,0x10,0x06,0x03,0x55,0x04,0x03,
	0x13,0x09,0x74,0x6c,0x73,0x2d,0x65,0x63,0x64,0x73,0x61,0x30,0x1e,0x17,0x0d,0x32,
	0x30,0x30,0x33,0x32,0x35,0x31,0x34,0x33,0x30,0x32,0x34,0x5a,0x17,0x0d,0x32,0x33,
	0x30,0x33,0x32,0x35,0x31,0x34,0x33,0x30,0x32,0x34,0x5a,0x30,0x36,0x31,0x0b,0x30,
	0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x43,0x48,0x31,0x13,0x30,0x11,0x06,0x03,
	0x55,0x04,0x0a,0x13,0x0a,0x73,0x74,0x72,0x6f,0x6e,0x67,0x53,0x77,0x61,0x6e,0x31,
	0x12,0x30,0x10,0x06,0x03,0x55,0x04,0x03,0x13,0x09,0x74,0x6c,0x73,0x2d,0x65,0x63,
	0x64,0x73,0x61,0x30,0x76,0x30,0x10,0x06,0x07,0x2a,0x86,0x48,0xce,0x3d,0x02,0x01,
	0x06,0x05,0x2b,0x81,0x04,0x00,0x22,0x03,0x62,0x00,0x04,0xd6,0xba,0xe1,0xf0,0x09,
	0x22,0x21,0x12,0x69,0xed,0x0e,0xd5,0x02,0x8c,0xb8,0x52,0xbb,0x57,0x68,0x0e,0xf3,
	0xdb,0xb9,0xb1,0xee,0x9c,0x67,0xa0,0xb8,0xdc,0x13,0x1e,0x5b,0x44,0x71,0x04,0xef,
	0x4e,0xe3,0xdd,0xf4,0xa6,0xc3,0xba,0x77,0x53,0xb8,0x28,0x5f,0xd2,0x97,0x05,0xa3,
	0x5b,0xe6,0xde,0x0a,0xce,0x11,0xa8,0xaf,0x02,0xbd,0xfa,0x17,0xf9,0xa7,0x38,0x3e,
	0x5b,0x57,0xb0,0x01,0xb3,0xc6,0x09,0x29,0x65,0xae,0xfb,0x87,0x92,0xa3,0xd7,0x3d,
	0x9a,0x1c,0x52,0x09,0xb1,0x47,0xc8,0xf6,0x18,0xbb,0x97,0xa3,0x13,0x30,0x11,0x30,
	0x0f,0x06,0x03,0x55,0x1d,0x11,0x04,0x08,0x30,0x06,0x87,0x04,0x7f,0x00,0x00,0x01,
	0x30,0x0a,0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,0x04,0x03,0x03,0x03,0x67,0x00,0x30,
	0x64,0x02,0x30,0x68,0x6e,0x8e,0x65,0x7c,0x84,0x33,0x82,0x2c,0x2e,0xca,0x17,0xe5,
	0x68,0xf7,0xbd,0x4b,0x6a,0xa2,0xa6,0xc3,0x40,0x21,0x4c,0xf0,0x00,0xd5,0xf7,0xfe,
	0x87,0x54,0xe3,0x2a,0x6f,0xce,0x46,0xf0,0x2e,0x9e,0x22,0xc3,0x73,0x47,0xf4,0xce,
	0x9b,0x03,0x0b,0x02,0x30,0x01,0x62,0x4c,0x6f,0x90,0x62,0x50,0x3b,0xe5,0x3f,0xdf,
	0x70,0x70,0xf6,0x81,0xad,0x25,0x2f,0x99,0x2e,0x69,0x8a,0x47,0xcf,0x14,0x31,0x27,
	0x1b,0x52,0xf4,0x91,0x78,0x0f,0x29,0x5b,0x33,0xca,0xb2,0x0c,0x45,0xee,0xc2,0x83,
	0xac,0x36,0x08,0x14,0x29,
};

/**
 * TLS certificate for Ed25519 key
 * pki --self --in ed25519.key --dn "C=CH, O=strongSwan, CN=tls-ed25519" \
 * --san 127.0.0.1
 */
static char ed25519_crt[] = {
	0x30,0x82,0x01,0x38,0x30,0x81,0xeb,0xa0,0x03,0x02,0x01,0x02,0x02,0x08,0x46,0x41,
	0x12,0x0d,0xa4,0xbd,0x00,0x11,0x30,0x05,0x06,0x03,0x2b,0x65,0x70,0x30,0x38,0x31,
	0x0b,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x43,0x48,0x31,0x13,0x30,0x11,
	0x06,0x03,0x55,0x04,0x0a,0x13,0x0a,0x73,0x74,0x72,0x6f,0x6e,0x67,0x53,0x77,0x61,
	0x6e,0x31,0x14,0x30,0x12,0x06,0x03,0x55,0x04,0x03,0x13,0x0b,0x74,0x6c,0x73,0x2d,
	0x65,0x64,0x32,0x35,0x35,0x31,0x39,0x30,0x1e,0x17,0x0d,0x32,0x30,0x31,0x30,0x32,
	0x32,0x31,0x34,0x30,0x31,0x30,0x30,0x5a,0x17,0x0d,0x32,0x33,0x31,0x30,0x32,0x32,
	0x31,0x34,0x30,0x31,0x30,0x30,0x5a,0x30,0x38,0x31,0x0b,0x30,0x09,0x06,0x03,0x55,
	0x04,0x06,0x13,0x02,0x43,0x48,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0a,0x13,
	0x0a,0x73,0x74,0x72,0x6f,0x6e,0x67,0x53,0x77,0x61,0x6e,0x31,0x14,0x30,0x12,0x06,
	0x03,0x55,0x04,0x03,0x13,0x0b,0x74,0x6c,0x73,0x2d,0x65,0x64,0x32,0x35,0x35,0x31,
	0x39,0x30,0x2a,0x30,0x05,0x06,0x03,0x2b,0x65,0x70,0x03,0x21,0x00,0xbd,0xbd,0xd2,
	0x9d,0x90,0x2c,0x4f,0xb0,0x0d,0x88,0xe9,0x92,0xba,0x59,0x91,0x6c,0x0a,0x30,0xc3,
	0x8e,0x7d,0x0d,0x55,0x67,0xf7,0xb0,0x37,0x39,0xfa,0x05,0x61,0xc3,0xa3,0x13,0x30,
	0x11,0x30,0x0f,0x06,0x03,0x55,0x1d,0x11,0x04,0x08,0x30,0x06,0x87,0x04,0x7f,0x00,
	0x00,0x01,0x30,0x05,0x06,0x03,0x2b,0x65,0x70,0x03,0x41,0x00,0x04,0xc6,0x12,0x57,
	0xfa,0x69,0x74,0xd2,0x3a,0x7d,0x1b,0x23,0xde,0x64,0x08,0xa8,0x05,0x75,0xd3,0x15,
	0xfb,0xd4,0x46,0xe0,0x4a,0x59,0x48,0x8e,0xee,0x4e,0x4d,0x72,0xbf,0xbc,0xdb,0x36,
	0xda,0x39,0x23,0x9a,0x06,0x88,0xee,0x63,0xe5,0xb4,0x23,0xf9,0xa9,0x80,0x41,0x99,
	0x3d,0x3f,0xb5,0x39,0x72,0x4b,0x62,0x86,0x4e,0x85,0x61,0x0b,
};

/**
 * TLS certificate for Ed448 key
 * pki --self --in ed448.key --dn "C=CH, O=strongSwan, CN=tls-ed448" \
 * --san 127.0.0.1
 */
static char ed448_crt[] = {
	0x30,0x82,0x01,0x80,0x30,0x82,0x01,0x00,0xa0,0x03,0x02,0x01,0x02,0x02,0x08,0x71,
	0xa9,0xa0,0xdd,0x5b,0xee,0xa0,0x5c,0x30,0x05,0x06,0x03,0x2b,0x65,0x71,0x30,0x36,
	0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x43,0x48,0x31,0x13,0x30,
	0x11,0x06,0x03,0x55,0x04,0x0a,0x13,0x0a,0x73,0x74,0x72,0x6f,0x6e,0x67,0x53,0x77,
	0x61,0x6e,0x31,0x12,0x30,0x10,0x06,0x03,0x55,0x04,0x03,0x13,0x09,0x74,0x6c,0x73,
	0x2d,0x65,0x64,0x34,0x34,0x38,0x30,0x1e,0x17,0x0d,0x32,0x30,0x31,0x30,0x32,0x33,
	0x32,0x30,0x34,0x34,0x30,0x35,0x5a,0x17,0x0d,0x32,0x33,0x31,0x30,0x32,0x33,0x32,
	0x30,0x34,0x34,0x30,0x35,0x5a,0x30,0x36,0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,
	0x06,0x13,0x02,0x43,0x48,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0a,0x13,0x0a,
	0x73,0x74,0x72,0x6f,0x6e,0x67,0x53,0x77,0x61,0x6e,0x31,0x12,0x30,0x10,0x06,0x03,
	0x55,0x04,0x03,0x13,0x09,0x74,0x6c,0x73,0x2d,0x65,0x64,0x34,0x34,0x38,0x30,0x43,
	0x30,0x05,0x06,0x03,0x2b,0x65,0x71,0x03,0x3a,0x00,0xdd,0x5e,0x19,0xc8,0x67,0xa9,
	0x93,0x53,0x5f,0x26,0xca,0x6f,0x2f,0xdf,0x9e,0x0f,0x48,0xb6,0x60,0x3e,0x56,0xa5,
	0xaf,0xe8,0xf9,0x3f,0xe5,0x1c,0xeb,0xf2,0xf4,0x84,0xd1,0x48,0xa9,0xb1,0x92,0x6b,
	0xa1,0x4d,0x47,0x86,0x8e,0xf9,0xcc,0xd2,0x58,0xd4,0x6f,0x8c,0x76,0x59,0xf4,0x77,
	0x59,0xc8,0x00,0xa3,0x13,0x30,0x11,0x30,0x0f,0x06,0x03,0x55,0x1d,0x11,0x04,0x08,
	0x30,0x06,0x87,0x04,0x7f,0x00,0x00,0x01,0x30,0x05,0x06,0x03,0x2b,0x65,0x71,0x03,
	0x73,0x00,0xc6,0xf0,0x4e,0xf2,0x41,0xe0,0xb4,0xa0,0x0a,0x9f,0x73,0x67,0xcb,0x89,
	0x97,0xf0,0x3a,0xfe,0x53,0xb2,0x1b,0x6c,0x37,0x24,0xbe,0x9e,0x2b,0x50,0x0c,0x98,
	0xb3,0x15,0x65,0x79,0x37,0xd6,0xc2,0x92,0x89,0x96,0xf3,0x5f,0x2d,0x70,0xa5,0x49,
	0xdd,0x7d,0x12,0x3d,0x17,0x28,0xd2,0x56,0x25,0xab,0x80,0x89,0x4d,0x6a,0xfa,0x32,
	0x6a,0x16,0x7f,0xd2,0x12,0xb3,0x73,0xf5,0xe7,0x1f,0x89,0x0b,0x5e,0x05,0xbe,0x69,
	0xb8,0x11,0x59,0xf9,0xdc,0x9c,0xca,0x68,0xe3,0x1f,0x7a,0x43,0x1a,0x72,0xfa,0x1f,
	0x07,0xe0,0x58,0xc2,0x3a,0x18,0xb3,0x01,0x68,0x90,0x40,0x05,0x7d,0x35,0x1a,0x3f,
	0xdc,0xc8,0x00,0x00
};

static void setup_credentials(chunk_t key_data, chunk_t cert_data)
{
	private_key_t *key;
	certificate_t *cert;

	creds = mem_cred_create();

	key = lib->creds->create(lib->creds, CRED_PRIVATE_KEY, KEY_RSA,
							 BUILD_BLOB, chunk_from_thing(rsa), BUILD_END);
	if (key)
	{
		creds->add_key(creds, key);
	}
	key = lib->creds->create(lib->creds, CRED_PRIVATE_KEY, KEY_ANY,
							 BUILD_BLOB, key_data, BUILD_END);
	if (key)
	{
		creds->add_key(creds, key);
	}
	cert = lib->creds->create(lib->creds, CRED_CERTIFICATE, CERT_X509,
							  BUILD_BLOB, chunk_from_thing(rsa_crt), BUILD_END);
	if (cert)
	{
		creds->add_cert(creds, TRUE, cert);
	}
	cert = lib->creds->create(lib->creds, CRED_CERTIFICATE, CERT_X509,
							  BUILD_BLOB, cert_data, BUILD_END);
	if (cert)
	{
		creds->add_cert(creds, TRUE, cert);
	}

	lib->credmgr->add_set(lib->credmgr, &creds->set);
}

START_SETUP(setup_creds)
{
	setup_credentials(chunk_from_thing(ecdsa), chunk_from_thing(ecdsa_crt));
}
END_SETUP

START_SETUP(setup_ed25519_creds)
{
	setup_credentials(chunk_from_thing(ed25519), chunk_from_thing(ed25519_crt));
}
END_SETUP

START_SETUP(setup_ed448_creds)
{
	setup_credentials(chunk_from_thing(ed448), chunk_from_thing(ed448_crt));
}
END_SETUP

START_TEARDOWN(teardown_creds)
{
	lib->credmgr->remove_set(lib->credmgr, &creds->set);
	creds->destroy(creds);
}
END_TEARDOWN

/**
 * Configuration for an echo server
 */
typedef struct {
	tls_version_t version;
	uint16_t port;
	char *addr;
	chunk_t data;
	int fd;
	bool cauth;
} echo_server_config_t;

/**
 * Run an echo server
 */
static job_requeue_t serve_echo(echo_server_config_t *config)
{
	tls_socket_t *tls;
	int sfd, cfd;
	identification_t *server, *client = NULL;
	ssize_t len, total, done;
	char buf[128];

	server = identification_create_from_string(config->addr);
	if (config->cauth)
	{
		client = server;
	}
	sfd = config->fd;
	while (TRUE)
	{
		cfd = accept(sfd, NULL, NULL);
		if (cfd < 0)
		{
			break;
		}

		tls = tls_socket_create(TRUE, server, client, cfd, NULL,
								TLS_1_0, config->version, TRUE);
		ck_assert(tls != NULL);

		while (TRUE)
		{
			len = tls->read(tls, buf, sizeof(buf), TRUE);
			if (len <= 0)
			{
				break;
			}
			total = 0;
			while (total < len)
			{
				done = tls->write(tls, buf + total, len - total);
				ck_assert_msg(done > 0, "%s", strerror(errno));
				total += done;
			}
		}

		tls->destroy(tls);
		close(cfd);
	}
	server->destroy(server);

	return JOB_REQUEUE_NONE;
}

/**
 * Start a echo server using config
 */
static void start_echo_server(echo_server_config_t *config)
{
	host_t *host;
	int on = 1;

	host = host_create_from_string(config->addr, config->port);

	config->fd = socket(AF_INET, SOCK_STREAM, 0);
	ck_assert(config->fd != -1);
	ck_assert(setsockopt(config->fd, SOL_SOCKET, SO_REUSEADDR,
						 (void*)&on, sizeof(on)) != -1);
	ck_assert_msg(bind(config->fd, host->get_sockaddr(host),
				  *host->get_sockaddr_len(host)) != -1, "%s", strerror(errno));
	host->destroy(host);
	ck_assert(listen(config->fd, 1) != -1);

	lib->processor->set_threads(lib->processor, 8);

	lib->processor->queue_job(lib->processor, (job_t*)
				callback_job_create((void*)serve_echo, config, NULL, NULL));
}

/**
 * Run client to perform echo test
 */
static void run_echo_client(echo_server_config_t *config)
{
	tls_socket_t *tls;
	ssize_t len, rd, wr;
	int fd;
	host_t *host;
	identification_t *server, *client = NULL;
	char buf[128];

	host = host_create_from_string(config->addr, config->port);
	server = identification_create_from_string(config->addr);
	if (config->cauth)
	{
		client = server;
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);
	ck_assert(fd != -1);
	ck_assert(connect(fd, host->get_sockaddr(host),
					  *host->get_sockaddr_len(host)) != -1);
	tls = tls_socket_create(FALSE, server, client, fd, NULL,
							TLS_1_0, config->version, TRUE);
	ck_assert(tls != NULL);

	wr = rd = 0;
	while (rd < config->data.len)
	{
		len = tls->write(tls, config->data.ptr + wr, config->data.len - wr);
		ck_assert(len >= 0);
		wr += len;

		len = tls->read(tls, buf, sizeof(buf), FALSE);
		if (len == -1 && errno == EWOULDBLOCK)
		{
			continue;
		}
		if (len == 0)
		{
			ck_assert_int_eq(rd, config->data.len);
			break;
		}
		ck_assert(len > 0);
		ck_assert(rd + len <= config->data.len);
		ck_assert(memeq(buf, config->data.ptr + rd, len));
		rd += len;
	}

	tls->destroy(tls);
	close(fd);
	host->destroy(host);
	server->destroy(server);
}

/**
 * Create server/peer configuration
 */
static echo_server_config_t *create_config(tls_version_t version, uint16_t port,
										   bool cauth)
{
	echo_server_config_t *config;

	INIT(config,
		.version = version,
		.addr = "127.0.0.1",
		.port = port,
		.cauth = cauth,
		.data = chunk_from_chars(0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08),
	);
	return config;
}

/**
 * Common test wrapper function for different test variants
 */
static void test_tls(tls_version_t version, uint16_t port, bool cauth, u_int i)
{
	echo_server_config_t *config;
	tls_cipher_suite_t *suites;
	char suite[128];
	int count;

	config = create_config(version, port, cauth);

	start_echo_server(config);

	count = tls_crypto_get_supported_suites(TRUE, version, &suites);
	ck_assert(i < count);
	snprintf(suite, sizeof(suite), "%N", tls_cipher_suite_names, suites[i]);
	lib->settings->set_str(lib->settings, "%s.tls.suites", suite, lib->ns);

	run_echo_client(config);

	free(suites);

	shutdown(config->fd, SHUT_RDWR);
	close(config->fd);

	free(config);
}

/**
 * TLS curve test wrapper function
 */
static void test_tls_curves(tls_version_t version, uint16_t port, bool cauth,
							u_int i)
{
	echo_server_config_t *config;
	diffie_hellman_group_t *groups;
	char curve[128];
	int count;

	config = create_config(version, port, cauth);

	start_echo_server(config);

	count = tls_crypto_get_supported_groups(&groups);
	ck_assert(i < count);
	snprintf(curve, sizeof(curve), "%N", diffie_hellman_group_names_short,
			 groups[i]);
	lib->settings->set_str(lib->settings, "%s.tls.curve", curve, lib->ns);

	run_echo_client(config);

	free(groups);

	shutdown(config->fd, SHUT_RDWR);
	close(config->fd);

	free(config);
}

/**
 * TLS server version test wrapper function
 */
static void test_tls_server(tls_version_t version, uint16_t port, bool cauth,
							u_int i)
{
	echo_server_config_t *client, *server;

	client = create_config(i, port, cauth);
	server = create_config(version, port, cauth);

	start_echo_server(server);

	run_echo_client(client);

	shutdown(client->fd, SHUT_RDWR);
	close(client->fd);
	shutdown(server->fd, SHUT_RDWR);
	close(server->fd);

	free(client);
	free(server);
}

/**
 * TLS client version test wrapper function
 */
static void test_tls_client(tls_version_t version, uint16_t port, bool cauth,
							u_int i)
{
	echo_server_config_t *client, *server;

	client = create_config(version, port, cauth);
	server = create_config(i, port, cauth);

	start_echo_server(server);

	run_echo_client(client);

	shutdown(client->fd, SHUT_RDWR);
	close(client->fd);
	shutdown(server->fd, SHUT_RDWR);
	close(server->fd);

	free(client);
	free(server);
}

START_TEST(test_tls_12_server)
{
	test_tls_server(TLS_1_2, 5665, FALSE, _i);
}
END_TEST

START_TEST(test_tls_13_server)
{
	test_tls_server(TLS_1_3, 5666, FALSE, _i);
}
END_TEST

START_TEST(test_tls_13_client)
{
	test_tls_client(TLS_1_3, 5667, FALSE, _i);
}
END_TEST

START_TEST(test_tls13_curves)
{
	test_tls_curves(TLS_1_3, 5668, FALSE, _i);
}
END_TEST

START_TEST(test_tls13)
{
	test_tls(TLS_1_3, 5669, FALSE, _i);
}
END_TEST

START_TEST(test_tls12)
{
	test_tls(TLS_1_2, 5671, FALSE, _i);
}
END_TEST

START_TEST(test_tls12_mutual)
{
	test_tls(TLS_1_2, 5672, TRUE, _i);
}
END_TEST

START_TEST(test_tls11)
{
	test_tls(TLS_1_1, 5673, FALSE, _i);
}
END_TEST

START_TEST(test_tls11_mutual)
{
	test_tls(TLS_1_1, 5674, TRUE, _i);
}
END_TEST

START_TEST(test_tls10)
{
	test_tls(TLS_1_0, 5675, FALSE, _i);
}
END_TEST

START_TEST(test_tls10_mutual)
{
	test_tls(TLS_1_0, 5676, TRUE, _i);
}
END_TEST

Suite *socket_suite_create()
{
	Suite *s;
	TCase *tc;

#define add_tls_test(func, version) \
	tcase_add_loop_test(tc, func, 0, \
						tls_crypto_get_supported_suites(TRUE, version, NULL));

#define add_tls_versions_test(func, from, to) \
	tcase_add_loop_test(tc, func, from, to+1);

	s = suite_create("socket");

	tc = tcase_create("TLS [1.0..1.3] client to TLS 1.3 server");
	tcase_add_checked_fixture(tc, setup_creds, teardown_creds);
	add_tls_versions_test(test_tls_13_server, TLS_1_0, TLS_1_3);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.3 client to TLS [1.0..1.3] server");
	tcase_add_checked_fixture(tc, setup_creds, teardown_creds);
	add_tls_versions_test(test_tls_13_client, TLS_1_0, TLS_1_3);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS [1.0..1.3] client to TLS 1.2 server");
	tcase_add_checked_fixture(tc, setup_creds, teardown_creds);
	add_tls_versions_test(test_tls_12_server, TLS_1_0, TLS_1_3);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.3/curves");
	tcase_add_checked_fixture(tc, setup_creds, teardown_creds);
	tcase_add_loop_test(tc, test_tls13_curves, 0,
						tls_crypto_get_supported_groups(NULL));
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.3/anon");
	tcase_add_checked_fixture(tc, setup_creds, teardown_creds);
	add_tls_test(test_tls13, TLS_1_3);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.2/anon");
	tcase_add_checked_fixture(tc, setup_creds, teardown_creds);
	add_tls_test(test_tls12, TLS_1_2);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.2/mutl");
	tcase_add_checked_fixture(tc, setup_creds, teardown_creds);
	add_tls_test(test_tls12_mutual, TLS_1_2);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.1/anon");
	tcase_add_checked_fixture(tc, setup_creds, teardown_creds);
	add_tls_test(test_tls11, TLS_1_1);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.1/mutl");
	tcase_add_checked_fixture(tc, setup_creds, teardown_creds);
	add_tls_test(test_tls11_mutual, TLS_1_1);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.0/anon");
	tcase_add_checked_fixture(tc, setup_creds, teardown_creds);
	add_tls_test(test_tls10, TLS_1_0);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.0/mutl");
	tcase_add_checked_fixture(tc, setup_creds, teardown_creds);
	add_tls_test(test_tls10_mutual, TLS_1_0);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.3/ed25519");
	tcase_add_checked_fixture(tc, setup_ed25519_creds, teardown_creds);
	add_tls_test(test_tls13, TLS_1_3);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.2/ed25519");
	tcase_add_checked_fixture(tc, setup_ed25519_creds, teardown_creds);
	add_tls_test(test_tls12, TLS_1_2);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.1/ed25519");
	tcase_add_checked_fixture(tc, setup_ed25519_creds, teardown_creds);
	add_tls_test(test_tls11, TLS_1_1);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.0/ed25519");
	tcase_add_checked_fixture(tc, setup_ed25519_creds, teardown_creds);
	add_tls_test(test_tls10, TLS_1_0);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.3/ed448");
	tcase_add_checked_fixture(tc, setup_ed448_creds, teardown_creds);
	add_tls_test(test_tls13, TLS_1_3);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.2/ed448");
	tcase_add_checked_fixture(tc, setup_ed448_creds, teardown_creds);
	add_tls_test(test_tls12, TLS_1_2);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.1/ed448");
	tcase_add_checked_fixture(tc, setup_ed448_creds, teardown_creds);
	add_tls_test(test_tls11, TLS_1_1);
	suite_add_tcase(s, tc);

	tc = tcase_create("TLS 1.0/ed448");
	tcase_add_checked_fixture(tc, setup_ed448_creds, teardown_creds);
	add_tls_test(test_tls10, TLS_1_0);
	suite_add_tcase(s, tc);

	return s;
}
