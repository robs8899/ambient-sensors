
#define DEFAULT_I2C_BUS  "/dev/i2c-1"
#define DEFAULT_SPI_DEV  "/dev/spidev0.0";
#define DEFAULT_LOG_DST  "./"
#define DEFAULT_LOG_INT  10

struct opt {
  char *dev_name;
  char *log_dst;
  int log_int;
};

int next_timepoint(int interval);
void interval_wait(int interval);
void sleep_wait(long millis);
int get_opts(int argc, char** argv, struct opt *opts);
int write_log(char *path, char *entry);
