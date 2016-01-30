
#ifndef __INTERVAL_H__
#define __INTERVAL_H__

/**
* The interval is used for the insertion into a decoder register. By allowing
* to definde lower and upper values, it is possible that e.g. a FrameID can be
* linked for a whole range.
*
*/
typedef struct Interval {

  /** the lower interval bound **/
  uint64_t lower;
  /** the upper interval bound **/
  uint64_t upper;

} Interval_t;

#endif
