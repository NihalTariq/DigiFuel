
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <windows.h>
#include <bcrypt.h>

#include "hash_password.h"

#define SALT_SIZE 16
#define HASH_SIZE 32
#define ITERATIONS 600000

// Convert hexadecimal string into bytes
static int hexToBytes(const char *hex,unsigned char *output,size_t outputSize)
{
    if (strlen(hex) != outputSize * 2)
        return 0;

    for (size_t i = 0; i < outputSize; i++)
    {
        unsigned int value;

        if (sscanf(hex + (i * 2), "%2x", &value) != 1)
            return 0;

        output[i] = (unsigned char)value;
    }

    return 1;
}

// Generate password hash
int hashPassword(
    const char *password,
    char *output,
    size_t outputSize
)
{
    BCRYPT_ALG_HANDLE hAlg = NULL;

    unsigned char salt[SALT_SIZE];
    unsigned char hash[HASH_SIZE];

    char saltHex[SALT_SIZE * 2 + 1];
    char hashHex[HASH_SIZE * 2 + 1];

    NTSTATUS status;

    status = BCryptOpenAlgorithmProvider(
        &hAlg,
        BCRYPT_SHA256_ALGORITHM,
        NULL,
        BCRYPT_ALG_HANDLE_HMAC_FLAG
    );

    if (status < 0)
        return 0;

    status = BCryptGenRandom(
        NULL,
        salt,
        SALT_SIZE,
        BCRYPT_USE_SYSTEM_PREFERRED_RNG
    );

    if (status < 0)
    {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return 0;
    }

    status = BCryptDeriveKeyPBKDF2(
        hAlg,
        (PUCHAR)password,
        (ULONG)strlen(password),
        salt,
        SALT_SIZE,
        ITERATIONS,
        hash,
        HASH_SIZE,
        0
    );

    BCryptCloseAlgorithmProvider(hAlg, 0);

    if (status < 0)
        return 0;

    for (int i = 0; i < SALT_SIZE; i++)
        sprintf(&saltHex[i * 2], "%02x", salt[i]);

    saltHex[SALT_SIZE * 2] = '\0';

    for (int i = 0; i < HASH_SIZE; i++)
        sprintf(&hashHex[i * 2], "%02x", hash[i]);

    hashHex[HASH_SIZE * 2] = '\0';

    int written = snprintf(
        output,
        outputSize,
        "pbkdf2_sha256$%d$%s$%s",
        ITERATIONS,
        saltHex,
        hashHex
    );

    memset(hash, 0, sizeof(hash));

    if (written < 0 || (size_t)written >= outputSize)
        return 0;

    return 1;
}

// Verify entered password
int verifyPassword(
    const char *password,
    const char *storedHash
)
{
    BCRYPT_ALG_HANDLE hAlg = NULL;

    unsigned char salt[SALT_SIZE];
    unsigned char expectedHash[HASH_SIZE];
    unsigned char actualHash[HASH_SIZE];

    char saltHex[SALT_SIZE * 2 + 1];
    char hashHex[HASH_SIZE * 2 + 1];

    const char *position;
    char *end;

    unsigned long iterations;

    NTSTATUS status;

    if (password == NULL || storedHash == NULL)
        return 0;

    // Check hash format
    if (strncmp(storedHash, "pbkdf2_sha256$", 14) != 0)
        return 0;

    // Read iteration count
    position = storedHash + 14;

    errno = 0;
    iterations = strtoul(position, &end, 10);

    if (errno == ERANGE ||
        end == position ||
        *end != '$' ||
        iterations == 0 ||
        iterations > 10000000)
    {
        return 0;
    }

    // Move to salt
    position = end + 1;

    // Check salt and hash lengths
    if (strlen(position) != 97 || position[32] != '$')
        return 0;

    memcpy(saltHex, position, 32);
    saltHex[32] = '\0';

    memcpy(hashHex, position + 33, 64);
    hashHex[64] = '\0';

    // Convert hexadecimal values into bytes
    if (!hexToBytes(saltHex, salt, SALT_SIZE) ||
        !hexToBytes(hashHex, expectedHash, HASH_SIZE))
    {
        return 0;
    }

    // Open SHA-256 algorithm
    status = BCryptOpenAlgorithmProvider(
        &hAlg,
        BCRYPT_SHA256_ALGORITHM,
        NULL,
        BCRYPT_ALG_HANDLE_HMAC_FLAG
    );

    if (status < 0)
        return 0;

    // Generate hash using the original salt
    status = BCryptDeriveKeyPBKDF2(
        hAlg,
        (PUCHAR)password,
        (ULONG)strlen(password),
        salt,
        SALT_SIZE,
        (ULONGLONG)iterations,
        actualHash,
        HASH_SIZE,
        0
    );

    BCryptCloseAlgorithmProvider(hAlg, 0);

    if (status < 0)
        return 0;

    // Compare both hashes
    volatile unsigned char difference = 0;

    for (int i = 0; i < HASH_SIZE; i++)
    {
        difference |= actualHash[i] ^ expectedHash[i];
    }

    memset(actualHash, 0, sizeof(actualHash));

    return difference == 0;
}