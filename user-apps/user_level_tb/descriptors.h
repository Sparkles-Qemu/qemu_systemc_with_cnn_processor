#ifndef DESCRIPTOS_H
#define DESCRIPTOS_H 

#include "io_access_lib.h"

#define RAM1_DESCRIPTORS	          3
#define RAM2_DESCRIPTORS	          3
#define RAM3_DESCRIPTORS	          3
#define BRANCH0_GROUP0_DESCRIPTORS  3
#define BRANCH0_GROUP1_DESCRIPTORS  3
#define BRANCH0_GROUP2_DESCRIPTORS  3
#define BRANCH1_GROUP0_DESCRIPTORS  3
#define BRANCH1_GROUP1_DESCRIPTORS  3
#define BRANCH1_GROUP2_DESCRIPTORS  3
#define BRANCH2_GROUP0_DESCRIPTORS  3
#define BRANCH2_GROUP1_DESCRIPTORS  3
#define BRANCH2_GROUP2_DESCRIPTORS  3
#define LOOPBACK_DESCRIPTORS 3

// Descriptor for source ram
struct Descriptor desc_mm2s = {0, 0, TRANSFER, BIG_RAM_SIZE, 1};

// Destination ram 1 descriptors
struct Descriptor ram1_descriptors[RAM1_DESCRIPTORS] = {
    {1, 0, WAIT, 1, 1},
    {2, 0, TRANSFER, SMALL_RAM_SIZE, 1},
	{0, 0, SUSPENDED, 0, 1}
    };

// Destination ram2 descriptors
struct Descriptor ram2_descriptors[RAM2_DESCRIPTORS] = {
	{1, 0, WAIT, 1 + SMALL_RAM_SIZE, 1},
	{2, 0, TRANSFER, SMALL_RAM_SIZE, 1},
	{0, 0, SUSPENDED, 0, 1}
    };

// Destination ram3 descriptors
struct Descriptor ram3_descriptors[RAM3_DESCRIPTORS] = {
	{1, 0, WAIT, 1 + 2 * SMALL_RAM_SIZE, 1},
	{2, 0, TRANSFER, SMALL_RAM_SIZE, 1},
	{0, 0, SUSPENDED, 0, 1}
    };

struct Descriptor branch0_group0_descriptors[BRANCH0_GROUP0_DESCRIPTORS] = {
	{1, 0, WAIT, BIG_RAM_SIZE, 1},
	{2, 0, TRANSFER, IMAGE_SIZE, 1},
	{2, 0, SUSPENDED, 0, 1}
    };

struct Descriptor branch0_group1_descriptors[BRANCH0_GROUP1_DESCRIPTORS] = {
	{1, 0, WAIT, BIG_RAM_SIZE + 3, 1}, // 2
	{2, 10, TRANSFER, IMAGE_SIZE, 1},
	{2, 0, SUSPENDED, 0, 1}
    };

struct Descriptor branch0_group2_descriptors[BRANCH0_GROUP2_DESCRIPTORS] = {
	{1, 0, WAIT, BIG_RAM_SIZE + 6, 1},
	{2, 20, TRANSFER, IMAGE_SIZE, 1},
	{2, 0, SUSPENDED, 0, 1}
    };

struct Descriptor branch1_group0_descriptors[BRANCH1_GROUP0_DESCRIPTORS] = {
	{1, 0, WAIT, BIG_RAM_SIZE + 9, 1},
	{2, 0, TRANSFER, IMAGE_SIZE, 1},
	{2, 0, SUSPENDED, 0, 1}
    };

struct Descriptor branch1_group1_descriptors[BRANCH1_GROUP1_DESCRIPTORS] = {
	{1, 0, WAIT, BIG_RAM_SIZE + 12, 1},
	{2, 10, TRANSFER, IMAGE_SIZE, 1},
	{2, 0, SUSPENDED, 0, 1}
    };

struct Descriptor branch1_group2_descriptors[BRANCH1_GROUP2_DESCRIPTORS] = {
	{1, 0, WAIT, BIG_RAM_SIZE + 15, 1},
	{2, 20, TRANSFER, IMAGE_SIZE, 1},
	{2, 0, SUSPENDED, 0, 1}
    };

struct Descriptor branch2_group0_descriptors[BRANCH2_GROUP0_DESCRIPTORS] = {
	{1, 0, WAIT, BIG_RAM_SIZE + 18, 1},
	{2, 0, TRANSFER, IMAGE_SIZE, 1},
	{2, 0, SUSPENDED, 0, 1}
    };

struct Descriptor branch2_group1_descriptors[BRANCH2_GROUP1_DESCRIPTORS] = {
    {1, 0, WAIT, BIG_RAM_SIZE + 21, 1},
	{2, 10, TRANSFER, IMAGE_SIZE, 1},
	{2, 0, SUSPENDED, 0, 1}
    };

struct Descriptor branch2_group2_descriptors[BRANCH2_GROUP1_DESCRIPTORS] = {
	{1, 0, WAIT, BIG_RAM_SIZE + 24, 1},
	{2, 20, TRANSFER, IMAGE_SIZE, 1},
	{2, 0, SUSPENDED, 0, 1}
    };

struct Descriptor loopback_descriptors[LOOPBACK_DESCRIPTORS] = {
	{1, 0, WAIT, BIG_RAM_SIZE + 27, 1},
	{2, 512, TRANSFER, 100, 1},
	{2, 0, SUSPENDED, 0, 1}
    }

#endif //DESCRIPTOS_H