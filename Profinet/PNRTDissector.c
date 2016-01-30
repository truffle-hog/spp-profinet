
struct PNRTDissector {
  struct Dissector dissector;


};

static const struct Dissector_ops PNRTDissector_ops = {

  sizeof(struct PNRTDissector), /* size */
  (uint64_t) 0x8892,
  (uint64_t) 0x8892,
  PNRT_free,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  PNRT_dissect,
};


Dissector_t *
PNRTDissector_new() {

  Dissector_t *dissector;
  struct PNRTDissector *pnrtDissector;

  dissector = Dissector_new(&PNRTDissector_ops);


}
