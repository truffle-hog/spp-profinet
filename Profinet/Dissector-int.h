
#ifndef __DISSECTOR_INT_H__
#define __DISSECTOR_INT_H__

/**
* The Base Dissector abstraction. Every implementation of a Dissector will use
* and implement the operations described in this interface.
* Dissector are used to dissect certain ranges of data in a network package,
* while having the possibility to link to further dissectors when the dissection
* of the desired range is complete.
*
* -> It is possible to link several Dissectors together building a tree of dissectors
* and subdissectors that call each other when their dissection part is completed.
*/
struct Dissector;

struct Dissector_ops {

  size_t Dissector_size;
  void (*Dissector_free)(Dissector_t *dissector);
  Dissector_t * (*Dissector_registerSub)(Dissector_t *this, Dissector_t *subDissector, Interval interval);
  Dissector_t * (*Dissector_getSub_64)(Dissector_t *this, uint64_t data);
  Dissector_t * (*Dissector_getSub_32)(Dissector_t *this, uint32_t data);
  Dissector_t * (*Dissector_getSub_16)(Dissector_t *this, uint16_t data);
  Dissector_t * (*Dissector_getSub_8)(Dissector_t *this, uint8_t data);
  Dissector_t * (*Dissector_getSub)(Dissector_t *this, void *data, int len);
  int (*Dissector_dissect)(Dissector_t *this, Buffer_t *buf, ProtocolTree_t *tree);
};

struct Dissector {

  /** wether this dissector was initialized **/
  bool initialized;

  /** the dissectors operations **/
  const struct Dissector_ops *ops;
  /** the dissector this dissector has been called from **/
  Dissector_t * calling;
}

Dissector_t *Dissector_new(const struct Dissector_ops *ops);

#endif
