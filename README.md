# Cr34mW4R3

Creamware is a simple ransomware for Proof-Of-Concept purposes. 

The malware uses AES-128b with SHA-256b as block hashing function.  
Encryption is offered by the ["Microsoft Enhanced RSA and AES Cryptographic Provider"](https://docs.microsoft.com/en-us/windows/win32/seccrypto/microsoft-aes-cryptographic-provider).

*__@Let's c.r.e.a.m.__* together!

Brought to you by the 0R1ngOCr3w - joint venture by [@k0z4c](https://github.com/cyb0tage) and [@Cyb0tage](https://github.com/cyb0tage).


## Usage

#### To Encrypt
```bash
creamware c:\dir\to\c.r.e.a.m. [aeskey16]
```
After encryption a README.txt file is generated on the user's dektop.

#### To Decrypt
```bash
creamware c:\dir\to\c.r.e.a.m. <aeskey> decrypt
```

If the default key was used, then issue the following
```bash
creamware.exe c:\dir\to\clean 3igcZhRdWq96m3GUmTAiv9 decrypt
```

### Notes:
Compiled with Windows Visual Studio 2022, C++11  
The README.txt note on Desktop will not be deleted after recovery

### Contributions 
Many thanks to my fella [@Cyb0tage](https://github.com/cyb0tage) for the ideas, contributions and the testing part.  
\
We got the feva <3


