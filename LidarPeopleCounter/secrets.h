#include <pgmspace.h>

#define SECRET
#define THING_NAME "xxxxxxxxxxx"
#define WIFI_SSID "xxxxxxxx-2G"
#define WIFI_PASSWORD "xxxxx"
#define AWS_IOT_ENDPOINT "xxxxxx"
#define AWS_IOT_PUBLISH_TOPIC "peopleCounter/pub"
#define DEVICE_ID -1

// Amazon Root CA 1
static char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----

)KEY";

// Device Private Key
static char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
)KEY";
