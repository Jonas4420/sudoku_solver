#include <preemptive_set.h>
#include <stdio.h>
#include <stdlib.h>

pset_t
char2pset(char c)
{
    pset_t result = 0;
    int color_index = -1;

    for(int i = 0 ; i < MAX_COLORS ; ++i)
    {
        if(color_table[i] == c)
        {
            color_index = i;
            break;
        }
    }

    if(color_index != -1)
    {
        result = 1;
        result <<= color_index;
    }

    return result;
}

void
pset2str(char string[MAX_COLORS + 1], pset_t pset)
{
    pset_t tmp = pset;
    int card = 0;

    for(int i = 0 ; i < MAX_COLORS ; ++i)
    {
        if((tmp % 2) == 1)
        {
            string[card] = color_table[i];
            ++card;
        }

        tmp >>= 1;
    }

    string[card] = '\0';
}

pset_t
pset_full(unsigned short color_range)
{
    if(color_range > MAX_COLORS)
        color_range = MAX_COLORS;

    return FULL >> (MAX_COLORS - color_range);
}

pset_t
pset_empty()
{
    return (pset_t) 0;
}

pset_t
pset_set(pset_t pset, char c)
{
    return pset | char2pset(c);
}

pset_t
pset_discard(pset_t pset, char c)
{
    return pset & (~char2pset(c));
}

pset_t
pset_substract(pset_t pset1, pset_t pset2)
{
    return pset1 & (~pset2);
}

bool
pset_equals(pset_t pset1, pset_t pset2)
{
    return pset1 == pset2;
}

pset_t
pset_negate(pset_t pset)
{
    return ~pset;
}

pset_t
pset_and(pset_t pset1, pset_t pset2)
{
    return pset1 & pset2;
}

pset_t
pset_or(pset_t pset1, pset_t pset2)
{
    return pset1 | pset2;
}

pset_t
pset_xor(pset_t pset1, pset_t pset2)
{
    return pset1 ^ pset2;
}

bool
pset_is_included(pset_t pset1, pset_t pset2)
{
    // Property of the inclusion
    return (pset1 & pset2) == pset1;
}

bool
pset_is_singleton(pset_t pset)
{
    // pset & (pset -1) is evaluated at 0 only if
    // pset is a singleton (or if pset is empty so
    // we have to test this case too)
    // As pset - 1 will set every bit at 1 before
    // the first bit set in pset
    // If there are more than one bit to 1,
    // pset - 1 will have some bits in common with pset
    // and the intersection will not be empty
    return (pset != 0) && ((pset & (pset - 1)) == 0);
}

unsigned short
pset_cardinality(pset_t pset)
{
    // Compute the Hamming weight of the preemptive set
    // The algorithm used is as it is defined on the page
    // http://en.wikipedia.org/wiki/Hamming_weight

    // Binary : 0101...
    const uint64_t m1 = 0x5555555555555555; 
    // Binary : 00110011...
    const uint64_t m2 = 0x3333333333333333;
    // Binary : 4 zeros, 4 ones ...
    const uint64_t m4 = 0x0f0f0f0f0f0f0f0f;
    // Binary : sum of 256 to the power of 0, 1, 2, ...
    const uint64_t h01 = 0x0101010101010101;

    // Put count of each 2 bits into those 2 bits
    pset -= (pset >> 1) & m1;
    // Put count of each 4 bits into those 4 bits
    pset = (pset & m2) + ((pset >> 2) & m2);
    // Put count of each 8 bits into those 8 bits
    pset = (pset + (pset >> 4)) & m4;

    // Return left 8 bits of
    // pset + (pset<<8) + (pset<<16) + (pset<<24) + ...
    return (pset * h01) >> 56;
}

pset_t
pset_leftmost(pset_t pset)
{
    pset_t result;

    result = pset - 1;
    result = pset & result;
    result = pset ^ result;

    return result;
}

pset_t
pset_n_leftmost(pset_t pset, int n)
{
    pset_t result = pset;

    if(n > 0)
    {
        for(int i = 0 ; i < (n - 1) ; ++i)
            result = pset_substract(result, pset_leftmost(result));

        result = pset_leftmost(result);
    }

    return result;
}
