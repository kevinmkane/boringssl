/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.] */

#ifndef OPENSSL_HEADER_EVP_INTERNAL_H
#define OPENSSL_HEADER_EVP_INTERNAL_H

#include <openssl/base.h>

#include <openssl/rsa.h>

#include <oqs/oqs.h>

#if defined(__cplusplus)
extern "C" {
#endif


struct evp_pkey_asn1_method_st {
  int pkey_id;
  uint8_t oid[11]; // OQS note: increased length (was 9) to accomodate larger PQ OIDs
  uint8_t oid_len;

  // pub_decode decodes |params| and |key| as a SubjectPublicKeyInfo
  // and writes the result into |out|. It returns one on success and zero on
  // error. |params| is the AlgorithmIdentifier after the OBJECT IDENTIFIER
  // type field, and |key| is the contents of the subjectPublicKey with the
  // leading padding byte checked and removed. Although X.509 uses BIT STRINGs
  // to represent SubjectPublicKeyInfo, every key type defined encodes the key
  // as a byte string with the same conversion to BIT STRING.
  int (*pub_decode)(EVP_PKEY *out, CBS *params, CBS *key);

  // pub_encode encodes |key| as a SubjectPublicKeyInfo and appends the result
  // to |out|. It returns one on success and zero on error.
  int (*pub_encode)(CBB *out, const EVP_PKEY *key);

  int (*pub_cmp)(const EVP_PKEY *a, const EVP_PKEY *b);

  // priv_decode decodes |params| and |key| as a PrivateKeyInfo and writes the
  // result into |out|. It returns one on success and zero on error. |params| is
  // the AlgorithmIdentifier after the OBJECT IDENTIFIER type field, and |key|
  // is the contents of the OCTET STRING privateKey field.
  int (*priv_decode)(EVP_PKEY *out, CBS *params, CBS *key);

  // priv_encode encodes |key| as a PrivateKeyInfo and appends the result to
  // |out|. It returns one on success and zero on error.
  int (*priv_encode)(CBB *out, const EVP_PKEY *key);

  int (*set_priv_raw)(EVP_PKEY *pkey, const uint8_t *in, size_t len);
  int (*set_pub_raw)(EVP_PKEY *pkey, const uint8_t *in, size_t len);
  int (*get_priv_raw)(const EVP_PKEY *pkey, uint8_t *out, size_t *out_len);
  int (*get_pub_raw)(const EVP_PKEY *pkey, uint8_t *out, size_t *out_len);

  // pkey_opaque returns 1 if the |pk| is opaque. Opaque keys are backed by
  // custom implementations which do not expose key material and parameters.
  int (*pkey_opaque)(const EVP_PKEY *pk);

  // OQS note: We've changed the return type from "int" to "size_t"
  // to allow for PQ algorithms with large signatures.
  size_t (*pkey_size)(const EVP_PKEY *pk);

  int (*pkey_bits)(const EVP_PKEY *pk);

  int (*param_missing)(const EVP_PKEY *pk);
  int (*param_copy)(EVP_PKEY *to, const EVP_PKEY *from);
  int (*param_cmp)(const EVP_PKEY *a, const EVP_PKEY *b);

  void (*pkey_free)(EVP_PKEY *pkey);
} /* EVP_PKEY_ASN1_METHOD */;


#define EVP_PKEY_OP_UNDEFINED 0
#define EVP_PKEY_OP_KEYGEN (1 << 2)
#define EVP_PKEY_OP_SIGN (1 << 3)
#define EVP_PKEY_OP_VERIFY (1 << 4)
#define EVP_PKEY_OP_VERIFYRECOVER (1 << 5)
#define EVP_PKEY_OP_ENCRYPT (1 << 6)
#define EVP_PKEY_OP_DECRYPT (1 << 7)
#define EVP_PKEY_OP_DERIVE (1 << 8)
#define EVP_PKEY_OP_PARAMGEN (1 << 9)

#define EVP_PKEY_OP_TYPE_SIG \
  (EVP_PKEY_OP_SIGN | EVP_PKEY_OP_VERIFY | EVP_PKEY_OP_VERIFYRECOVER)

#define EVP_PKEY_OP_TYPE_CRYPT (EVP_PKEY_OP_ENCRYPT | EVP_PKEY_OP_DECRYPT)

#define EVP_PKEY_OP_TYPE_NOGEN \
  (EVP_PKEY_OP_SIG | EVP_PKEY_OP_CRYPT | EVP_PKEY_OP_DERIVE)

#define EVP_PKEY_OP_TYPE_GEN (EVP_PKEY_OP_KEYGEN | EVP_PKEY_OP_PARAMGEN)

// EVP_PKEY_CTX_ctrl performs |cmd| on |ctx|. The |keytype| and |optype|
// arguments can be -1 to specify that any type and operation are acceptable,
// otherwise |keytype| must match the type of |ctx| and the bits of |optype|
// must intersect the operation flags set on |ctx|.
//
// The |p1| and |p2| arguments depend on the value of |cmd|.
//
// It returns one on success and zero on error.
OPENSSL_EXPORT int EVP_PKEY_CTX_ctrl(EVP_PKEY_CTX *ctx, int keytype, int optype,
                                     int cmd, int p1, void *p2);

#define EVP_PKEY_CTRL_MD 1
#define EVP_PKEY_CTRL_GET_MD 2

// EVP_PKEY_CTRL_PEER_KEY is called with different values of |p1|:
//   0: Is called from |EVP_PKEY_derive_set_peer| and |p2| contains a peer key.
//      If the return value is <= 0, the key is rejected.
//   1: Is called at the end of |EVP_PKEY_derive_set_peer| and |p2| contains a
//      peer key. If the return value is <= 0, the key is rejected.
//   2: Is called with |p2| == NULL to test whether the peer's key was used.
//      (EC)DH always return one in this case.
//   3: Is called with |p2| == NULL to set whether the peer's key was used.
//      (EC)DH always return one in this case. This was only used for GOST.
#define EVP_PKEY_CTRL_PEER_KEY 3

// EVP_PKEY_ALG_CTRL is the base value from which key-type specific ctrl
// commands are numbered.
#define EVP_PKEY_ALG_CTRL 0x1000

#define EVP_PKEY_CTRL_RSA_PADDING (EVP_PKEY_ALG_CTRL + 1)
#define EVP_PKEY_CTRL_GET_RSA_PADDING (EVP_PKEY_ALG_CTRL + 2)
#define EVP_PKEY_CTRL_RSA_PSS_SALTLEN (EVP_PKEY_ALG_CTRL + 3)
#define EVP_PKEY_CTRL_GET_RSA_PSS_SALTLEN (EVP_PKEY_ALG_CTRL + 4)
#define EVP_PKEY_CTRL_RSA_KEYGEN_BITS (EVP_PKEY_ALG_CTRL + 5)
#define EVP_PKEY_CTRL_RSA_KEYGEN_PUBEXP (EVP_PKEY_ALG_CTRL + 6)
#define EVP_PKEY_CTRL_RSA_OAEP_MD (EVP_PKEY_ALG_CTRL + 7)
#define EVP_PKEY_CTRL_GET_RSA_OAEP_MD (EVP_PKEY_ALG_CTRL + 8)
#define EVP_PKEY_CTRL_RSA_MGF1_MD (EVP_PKEY_ALG_CTRL + 9)
#define EVP_PKEY_CTRL_GET_RSA_MGF1_MD (EVP_PKEY_ALG_CTRL + 10)
#define EVP_PKEY_CTRL_RSA_OAEP_LABEL (EVP_PKEY_ALG_CTRL + 11)
#define EVP_PKEY_CTRL_GET_RSA_OAEP_LABEL (EVP_PKEY_ALG_CTRL + 12)
#define EVP_PKEY_CTRL_EC_PARAMGEN_CURVE_NID (EVP_PKEY_ALG_CTRL + 13)

struct evp_pkey_ctx_st {
  // Method associated with this operation
  const EVP_PKEY_METHOD *pmeth;
  // Engine that implements this method or NULL if builtin
  ENGINE *engine;
  // Key: may be NULL
  EVP_PKEY *pkey;
  // Peer key for key agreement, may be NULL
  EVP_PKEY *peerkey;
  // operation contains one of the |EVP_PKEY_OP_*| values.
  int operation;
  // Algorithm specific data
  void *data;
} /* EVP_PKEY_CTX */;

struct evp_pkey_method_st {
  int pkey_id;

  int (*init)(EVP_PKEY_CTX *ctx);
  int (*copy)(EVP_PKEY_CTX *dst, EVP_PKEY_CTX *src);
  void (*cleanup)(EVP_PKEY_CTX *ctx);

  int (*keygen)(EVP_PKEY_CTX *ctx, EVP_PKEY *pkey);

  int (*sign)(EVP_PKEY_CTX *ctx, uint8_t *sig, size_t *siglen,
              const uint8_t *tbs, size_t tbslen);

  int (*sign_message)(EVP_PKEY_CTX *ctx, uint8_t *sig, size_t *siglen,
                      const uint8_t *tbs, size_t tbslen);

  int (*verify)(EVP_PKEY_CTX *ctx, const uint8_t *sig, size_t siglen,
                const uint8_t *tbs, size_t tbslen);

  int (*verify_message)(EVP_PKEY_CTX *ctx, const uint8_t *sig, size_t siglen,
                        const uint8_t *tbs, size_t tbslen);

  int (*verify_recover)(EVP_PKEY_CTX *ctx, uint8_t *out, size_t *out_len,
                        const uint8_t *sig, size_t sig_len);

  int (*encrypt)(EVP_PKEY_CTX *ctx, uint8_t *out, size_t *outlen,
                 const uint8_t *in, size_t inlen);

  int (*decrypt)(EVP_PKEY_CTX *ctx, uint8_t *out, size_t *outlen,
                 const uint8_t *in, size_t inlen);

  int (*derive)(EVP_PKEY_CTX *ctx, uint8_t *key, size_t *keylen);

  int (*paramgen)(EVP_PKEY_CTX *ctx, EVP_PKEY *pkey);

  int (*ctrl)(EVP_PKEY_CTX *ctx, int type, int p1, void *p2);
} /* EVP_PKEY_METHOD */;

typedef struct {
  union {
    uint8_t priv[64];
    struct {
      // Shift the location of the public key to align with where it is in the
      // private key representation.
      uint8_t pad[32];
      uint8_t value[32];
    } pub;
  } key;
  char has_private;
} ED25519_KEY;

typedef struct {
    OQS_SIG *ctx;
    uint8_t *pub;
    uint8_t *priv;
    char has_private;
} OQS_KEY;

typedef struct {
  uint8_t pub[32];
  uint8_t priv[32];
  char has_private;
} X25519_KEY;

extern const EVP_PKEY_ASN1_METHOD dsa_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD ec_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD rsa_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD ed25519_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD x25519_asn1_meth;
///// OQS_TEMPLATE_FRAGMENT_DECLARE_ASN1_METHS_START
extern const EVP_PKEY_ASN1_METHOD oqs_sig_default_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD dilithium2_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD dilithium3_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD dilithium4_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD falcon512_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD falcon1024_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD mqdss3148_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD mqdss3164_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD picnicl1fs_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD picnicl1ur_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD picnicl1full_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD picnic3l1_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD picnic3l3_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD picnic3l5_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD qteslapi_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD qteslapiii_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD rainbowIaclassic_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD rainbowIacyclic_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD rainbowIacycliccompressed_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD rainbowIIIcclassic_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD rainbowIIIccyclic_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD rainbowIIIccycliccompressed_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD rainbowVcclassic_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD rainbowVccyclic_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD rainbowVccycliccompressed_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka128frobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka128fsimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka128srobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka128ssimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka192frobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka192fsimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka192srobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka192ssimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka256frobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka256fsimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka256srobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsharaka256ssimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256128frobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256128fsimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256128srobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256128ssimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256192frobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256192fsimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256192srobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256192ssimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256256frobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256256fsimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256256srobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincssha256256ssimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256128frobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256128fsimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256128srobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256128ssimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256192frobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256192fsimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256192srobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256192ssimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256256frobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256256fsimple_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256256srobust_asn1_meth;
extern const EVP_PKEY_ASN1_METHOD sphincsshake256256ssimple_asn1_meth;
///// OQS_TEMPLATE_FRAGMENT_DECLARE_ASN1_METHS_END

extern const EVP_PKEY_METHOD rsa_pkey_meth;
extern const EVP_PKEY_METHOD ec_pkey_meth;
extern const EVP_PKEY_METHOD ed25519_pkey_meth;
extern const EVP_PKEY_METHOD x25519_pkey_meth;
///// OQS_TEMPLATE_FRAGMENT_DECLARE_PKEY_METHS_START
extern const EVP_PKEY_METHOD oqs_sig_default_pkey_meth;
extern const EVP_PKEY_METHOD dilithium2_pkey_meth;
extern const EVP_PKEY_METHOD dilithium3_pkey_meth;
extern const EVP_PKEY_METHOD dilithium4_pkey_meth;
extern const EVP_PKEY_METHOD falcon512_pkey_meth;
extern const EVP_PKEY_METHOD falcon1024_pkey_meth;
extern const EVP_PKEY_METHOD mqdss3148_pkey_meth;
extern const EVP_PKEY_METHOD mqdss3164_pkey_meth;
extern const EVP_PKEY_METHOD picnicl1fs_pkey_meth;
extern const EVP_PKEY_METHOD picnicl1ur_pkey_meth;
extern const EVP_PKEY_METHOD picnicl1full_pkey_meth;
extern const EVP_PKEY_METHOD picnic3l1_pkey_meth;
extern const EVP_PKEY_METHOD picnic3l3_pkey_meth;
extern const EVP_PKEY_METHOD picnic3l5_pkey_meth;
extern const EVP_PKEY_METHOD qteslapi_pkey_meth;
extern const EVP_PKEY_METHOD qteslapiii_pkey_meth;
extern const EVP_PKEY_METHOD rainbowIaclassic_pkey_meth;
extern const EVP_PKEY_METHOD rainbowIacyclic_pkey_meth;
extern const EVP_PKEY_METHOD rainbowIacycliccompressed_pkey_meth;
extern const EVP_PKEY_METHOD rainbowIIIcclassic_pkey_meth;
extern const EVP_PKEY_METHOD rainbowIIIccyclic_pkey_meth;
extern const EVP_PKEY_METHOD rainbowIIIccycliccompressed_pkey_meth;
extern const EVP_PKEY_METHOD rainbowVcclassic_pkey_meth;
extern const EVP_PKEY_METHOD rainbowVccyclic_pkey_meth;
extern const EVP_PKEY_METHOD rainbowVccycliccompressed_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka128frobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka128fsimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka128srobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka128ssimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka192frobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka192fsimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka192srobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka192ssimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka256frobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka256fsimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka256srobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsharaka256ssimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256128frobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256128fsimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256128srobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256128ssimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256192frobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256192fsimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256192srobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256192ssimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256256frobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256256fsimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256256srobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincssha256256ssimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256128frobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256128fsimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256128srobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256128ssimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256192frobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256192fsimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256192srobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256192ssimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256256frobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256256fsimple_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256256srobust_pkey_meth;
extern const EVP_PKEY_METHOD sphincsshake256256ssimple_pkey_meth;
///// OQS_TEMPLATE_FRAGMENT_DECLARE_PKEY_METHS_END

#if defined(__cplusplus)
}  // extern C
#endif

#endif  // OPENSSL_HEADER_EVP_INTERNAL_H
