#include <pgmspace.h>

#define SECRET
#define THING_NAME "aws_iot_core_thing_name"
#define WIFI_SSID "your_2g_network_here"
#define WIFI_PASSWORD "your_wifi_password_here"
#define AWS_IOT_ENDPOINT "your_aws_endpoint_here"
#define AWS_IOT_PUBLISH_TOPIC "peopleCounter/pub" // your pub topic here

// Amazon Root CA 1
static char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
Your Root CA Cert here
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
Your device cert here
-----END CERTIFICATE-----

)KEY";

// Device Private Key
static char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
Your RSA private key here
-----END RSA PRIVATE KEY-----
)KEY";
