# Programming Assignment 5: OTP

> **Due:** Mar 16 by 11:59pm | **Points:** 150

## Introduction

Create five small programs that encrypt and decrypt information using a **one-time pad-like system**. These programs combine multi-processing with socket-based IPC, and are accessible from the command line using standard Unix features like I/O redirection and job control.

---

## Learning Outcomes

- Explain the Client-Server communication model (Module 8, MLO 1)
- Use the programmer's view of the internet to design network programs (Module 8, MLO 3)
- Explain Unix sockets (Module 8, MLO 4)
- Design and implement client/server programs using sockets (Module 8, MLO 5)
- Compare and evaluate server designs (Module 8, MLO 6)

---

## One-Time Pads

Use **modulo 27** operations (26 capital letters + space).

### Encryption Example

```
      H       E       L       L       O   message
   7 (H)   4 (E)  11 (L)  11 (L)  14 (O) message values
+ 23 (X)  12 (M)   2 (C)  10 (K)  11 (L) key values
= 30      16      13      21      25      message + key
=  4 (E)  16 (Q)  13 (N)  21 (V)  25 (Z) mod 26
      E       Q       N       V       Z   → ciphertext
```

### Decryption

Subtract the key from the ciphertext (mod 26). If negative, add 26.

---

## The Five Programs

### `enc_server`

- Encryption daemon running in the background
- Listens on a specified port
- `accept()` connections → fork child to handle each client
- Child verifies connection is from `enc_client`
- Receives plaintext + key → sends back ciphertext
- Supports **up to 5 concurrent** connections

```bash
$ enc_server 57171 &
```

### `enc_client`

- Connects to `enc_server` and requests encryption

```bash
enc_client plaintext key port
enc_client plaintext key port > myciphertext
enc_client plaintext key port > myciphertext &
```

- Validates input: rejects bad characters or short keys (exit 1)
- Must **not** connect to `dec_server` (exit 2 on rejection)

### `dec_server`

Same as `enc_server` but performs **decryption**. Returns plaintext to `dec_client`.

### `dec_client`

Same as `enc_client` but connects to `dec_server` for decryption. Must **not** connect to `enc_server`.

### `keygen`

Generates a random key of specified length using the 27 allowed characters. Last character is a newline.

```bash
$ keygen 256 > mykey
```

---

## Example Usage

```bash
$ cat plaintext1
THE RED GOOSE FLIES AT MIDNIGHT STOP
$ enc_server 57171 &
$ dec_server 57172 &
$ keygen 1024 > mykey
$ enc_client plaintext1 mykey 57171 > ciphertext1
$ cat ciphertext1
WANAWTRLFTH RAAQGZSOHCTYS JDBEGYZQDQ
$ dec_client ciphertext1 mykey 57172 > plaintext1_a
$ cat plaintext1_a
THE RED GOOSE FLIES AT MIDNIGHT STOP
$ cmp plaintext1 plaintext1_a
$ echo $?
0
```

---

## Hints

- **Partial sends/receives:** Wrap `send()` / `recv()` in loops — data may not transfer in one call
- **Termination markers:** Send message length first, or use a termination character
- **Port reuse:** Use `setsockopt()` with `SO_REUSEADDR`
- **Newlines:** Strip newlines before encrypting; add them back when outputting
- **Use localhost** as the target IP for all programs

---

## Compilation Script

```bash
#!/bin/bash
gcc --std=gnu99 -o enc_server enc_server.c
gcc --std=gnu99 -o enc_client enc_client.c
gcc --std=gnu99 -o dec_server dec_server.c
gcc --std=gnu99 -o dec_client dec_client.c
gcc --std=gnu99 -o keygen keygen.c
```

---

## Submission

- Submit: `youronid_program5.zip` containing all source files
- Must produce 5 executables: `enc_server`, `enc_client`, `dec_server`, `dec_client`, `keygen`
- Graded via **Gradescope**
