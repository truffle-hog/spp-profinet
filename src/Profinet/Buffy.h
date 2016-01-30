uint8_t Buffy_get_uint8_t(Buffy_t *buffy, const int offset);

uint16_t Buffy_get_ntohs(Buffy_t *buffy, const int offset);
uint32_t Buffy_get_ntoh24(Buffy_t *buffy, const int offset);
uint32_t Buffy_get_ntohl(Buffy_t *buffy, const int offset);
uint64_t Buffy_get_ntoh40(Buffy_t *buffy, const int offset);
int64_t Buffy_get_ntohi40(Buffy_t *buffy, const int offset);
uint64_t Buffy_get_ntoh48(Buffy_t *buffy, const int offset);
int64_t Buffy_get_ntohi48(Buffy_t *buffy, const int offset);
uint64_t Buffy_get_ntoh56(Buffy_t *buffy, const int offset);
int64_t Buffy_get_ntohi56(Buffy_t *buffy, const int offset);
uint64_t Buffy_get_ntoh64(Buffy_t *buffy, const int offset);
float Buffy_get_ntohieee_float(Buffy_t *buffy, const int offset);
double Buffy_get_ntohieee_double(Buffy_t *buffy,
    const int offset);

uint16_t Buffy_get_letohs(Buffy_t *buffy, const int offset);
uint32_t Buffy_get_letoh24(Buffy_t *buffy, const int offset);
uint32_t Buffy_get_letohl(Buffy_t *buffy, const int offset);
uint64_t Buffy_get_letoh40(Buffy_t *buffy, const int offset);
int64_t Buffy_get_letohi40(Buffy_t *buffy, const int offset);
uint64_t Buffy_get_letoh48(Buffy_t *buffy, const int offset);
int64_t Buffy_get_letohi48(Buffy_t *buffy, const int offset);
uint64_t Buffy_get_letoh56(Buffy_t *buffy, const int offset);
int64_t Buffy_get_letohi56(Buffy_t *buffy, const int offset);
uint64_t Buffy_get_letoh64(Buffy_t *buffy, const int offset);
float Buffy_get_letohieee_float(Buffy_t *buffy, const int offset);
double Buffy_get_letohieee_double(Buffy_t *buffy,
    const int offset);

uint16_t Buffy_get_uint16_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
uint32_t Buffy_get_uint24_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
uint32_t Buffy_get_uint32_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
uint64_t Buffy_get_uint40_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
int64_t Buffy_get_int40_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
uint64_t Buffy_get_uint48_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
int64_t Buffy_get_int48_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
uint64_t Buffy_get_uint56_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
int64_t Buffy_get_int56_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
uint64_t Buffy_get_uint64_t(Buffy_t *buffy, const int offset, const unsigned int encoding);
float Buffy_get_ieee_float(Buffy_t *buffy, const int offset, const unsigned int encoding);
double Buffy_get_ieee_double(Buffy_t *buffy, const int offset, const unsigned int encoding);
