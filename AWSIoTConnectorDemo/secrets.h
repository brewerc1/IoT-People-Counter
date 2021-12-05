#include <pgmspace.h>

#define SECRET
// Populate your info below
#define THING_NAME "esp32_demo"
#define WIFI_SSID "xxxxxxxxxxxxxxxxxx"
#define WIFI_PASSWORD "xxxxxxxx"
#define AWS_IOT_ENDPOINT "xxxxxxxxxxx.us-east-2.amazonaws.com"
#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"

// Amazon Root CA 1
static char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
xxxxxxxxxxxxxxxxxxxxxxxxxxx
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
xxxxxxxxxxxxxxxxxxxxxxxxx
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
-----END RSA PRIVATE KEY-----
)KEY";
