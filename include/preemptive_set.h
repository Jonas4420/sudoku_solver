/* PREEMPTIVE_SET_H */
#ifndef PREEMPTIVE_SET_H
#define PREEMPTIVE_SET_H

#include <inttypes.h>
#include <stdbool.h>

#define MAX_COLORS 64
// pset with every color set to 1
#define FULL (((pset_t) 0) -1)

static const char color_table[] = \
                           "123456789"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz"
                           "@&*";

typedef uint64_t pset_t;

// Convert a character into a preemptive set
// Complexity : for n colors O(n)
// Parameter : character corresponding to the desired color
// Return : pset with the corresponding color set to 1 and others to 0
pset_t char2pset(char);

// Convert a preemptive set into a string of characters
// Complexity : for n colors O(n)
// Parameter : the string that will contain the string representation
//             of the pset. it has to have enough space (MAX_COLOR + 1 max)
// Parameter : the pset to convert into a string of characters
void pset2str(char[MAX_COLORS + 1], pset_t);

// Return the full pset of a certain range
// Complexity : for n colors O(1)
// Parameter : the range for the initialization of the pset
//             if the range is greater than MAX_COLOR then it set every
//             colors to 1
// Return : the pset with every color of a range set to 1
pset_t pset_full(unsigned short);

// Return the empty set
// Complexity : for n colors O(1)
// Return : the  pset with every color set to 0
pset_t pset_empty();

// Set a color on a pset
// Complexity : for n colors O(n)
// Parameter : the considered pset to work on
// Parameter : the character corresponding to the color to set
// Return : a copy of the pset with the color set to 1
pset_t pset_set(pset_t, char);

// Reset a color on a pset
// Complexity : for n colors O(n)
// Parameter : the considered pset to work on
// Parameter : the character corresponding to the color to reset
// Return : a copy of the pset with the color set to 0
pset_t pset_discard(pset_t, char);

// Substract a pset to an other one (pset1 \ pset2)
// Complexity : for n colors O(1)
// Parameter : the first pset to work on
// Parameter : the second pset to substract
// Return : a new pset where all colors of pset1 are set to 1 excepted
//          those that were also set in pset2
pset_t pset_substract(pset_t, pset_t);

// Test the equality of two psets
// Complexity : for n colors O(1)
// Parameter : the first pset
// Parameter : the second pset
// Return : true if the two psets are equals, false in the other case
bool pset_equals(pset_t, pset_t);

// Make the absolute complement of a pset
// Complexity : for n colors O(1)
// Parameter : the considered pset
// Return  : a new set where all colors set of the pset are reset
//           and every colors unset of the pset are set
pset_t pset_negate(pset_t);

// Make the intersection of two psets
// Complexity : for n colors O(1)
// Parameter : the first pset
// Parameter : the second pset
// Return : a new pset where a color is set only if it is set in
//          the first pset AND in the second pset
pset_t pset_and(pset_t, pset_t);

// Make the union of two psets
// Complexity : for n colors O(1)
// Parameter : the first pset
// Parameter : the second pset
// Return : a new pset where a color is set if it is set in the
//          first pset OR if it is set in the second pset
pset_t pset_or(pset_t, pset_t);

// Make the strict union of two psets
// Complexity : for n colors O(1)
// Parameter : the first pset
// Parameter : the second pset
// Return : a new pset where a color is set if it is set in the
//          first pset OR if it is set in the second pset but not
//          in both at the same time
pset_t pset_xor(pset_t, pset_t);

// Test is the first pset is included in the second pset
// Complexity : for n colors O(1)
// Parameter : the first pset
// Parameter : the second pset
// Return : true if pset1 is totaly included in pset2, false otherwise
bool pset_is_included(pset_t, pset_t);

// Test is the pset has a single element
// Complexity : for n colors O(1)
// Parameter : the pset to test
// Return : true if the pset is a singleton, false in the other case
bool pset_is_singleton(pset_t);

// Compute the number of colors that are in the set
// Complexity : for n colors O(log n)
// Parameter : the considerated pset
// Return : the cardinality of the pset
unsigned short pset_cardinality(pset_t);

// Return the leftmost color from a pset
// Complexity : for n colors O(1)
// Parameter : the considered pset
// Return : a new pset that is the singleton of the leftmost color
//          if the pset is empty, return a new empty pset
pset_t pset_leftmost(pset_t);

// Return the nth leftmost color from a pset
// Complexity : for n colors O(n)
// Parameter : the considered pset
// Parameter : the number of the color
// Return : a new pset that is the singleton of the nth leftmost color
//          if n equals 0, then it returns the considered pset
pset_t pset_n_leftmost(pset_t, int);

#endif
