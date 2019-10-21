#define DHT_MAX_ATTEMPTS 10

#define DHT11_TYPE 0
#define DHT22_TYPE 1

/** needs also timer_init()! **/
void dht_init(int port, int pin);
int dht_read_raw(int type, uint8_t *buffer);
void dht_read(int type, int *rh, int *temp, int *fails);
