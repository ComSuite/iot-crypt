#include "iot-crypt.h"
#include "esp_hw_wdt.h"
#include <time.h>
#include <math.h>

char* test = "You may obtain a copy of the License at http:////www.apache.org//licenses//LICENSE-2.0 Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied";
char output[2048];
char decrypted[2048];
unsigned int key_size = 1024; //bit
unsigned int block_size = (key_size / 8) - 11;
mbedtls_rsa_context ctx;

int simple_rand(void *p_rng, unsigned char *output, size_t output_len)
{
  for (size_t i = 0; i < output_len; i++)
  {
    output[i] = (unsigned char)rand();
  }

  return 0;
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  time_t t;

  Serial.println("Starting");
  srand((unsigned)time(&t));
  Serial.println("Initializing RSA");
  
  mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V15, 0);
  Serial.println("Generating keys");

  ESP.wdtDisable();
  esp_hw_wdt_disable();
  
  int res = mbedtls_rsa_gen_key(&ctx, simple_rand, NULL, key_size, 65537);      
  //mbedtls_rsa_self_test(1);
  
  esp_hw_wdt_enable();
  ESP.wdtEnable(1000);
  
  Serial.println("Started");
}

void loop() {
  printf("Loop\n");
  memset(output, 0x00, sizeof(output));

//  wdt_disable();
//  esp_hw_wdt_disable();
  
  printf("Plain text len: %d\n", block_size);
  int res = mbedtls_rsa_pkcs1_encrypt(&ctx, simple_rand, NULL, MBEDTLS_RSA_PUBLIC, block_size, (const unsigned char*)test, (unsigned char*)output);

  size_t len = 0;
  memset(decrypted, 0x00, sizeof(decrypted));
  if (mbedtls_rsa_pkcs1_decrypt(&ctx, simple_rand, NULL, MBEDTLS_RSA_PRIVATE, &len, (unsigned char*)output, (unsigned char*)decrypted, sizeof(decrypted)) == 0) {
    printf("Decrypted length: %d\n", len);
    printf("%s\n", decrypted);
  }
  else
    printf("Failed!\n");
  
//  esp_hw_wdt_enable();
//  wdt_enable(1000);

  delay(100);
}
