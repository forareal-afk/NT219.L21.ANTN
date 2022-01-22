import string
def Encryption(plaintext, key):
    # Tiền xử lý
    key = key.lower()
    key = "".join(key.split())
    plain = plaintext.lower()
    plain = "".join(plain.split())
    # Padding
    k = key
    while len(key) < len(plain):
        key = key + k
    key = key[0:len(plain)]
    # Mã hóa
    cipher = ""
    for i in range(len(plain)):
        k = (string.ascii_lowercase.index(plain[i]) + string.ascii_lowercase.index(key[i])) % 26
        k = string.ascii_lowercase[k]
        cipher+=k
    # Hậu xử lý
    i = 0
    s = ""
    for x in plaintext:
        if x == ' ':
            s +=" "
            continue
        if x <'a':
            s += cipher[i].upper()
        else:
            s += cipher[i]
        i+=1
    return s
def Decryption(ciphertext, key):
    # Tiền xử lý
    key = key.lower()
    key = "".join(key.split())
    cipher = ciphertext.lower()
    cipher = "".join(cipher.split())
    # Padding
    k = key
    while len(key) < len(cipher):
        key = key + k
    key = key[0:len(cipher)]
    # Giải mã
    plain = ""
    for i in range(len(cipher)):
        k = (string.ascii_lowercase.index(cipher[i]) - string.ascii_lowercase.index(key[i])) % 26
        k = string.ascii_lowercase[k]
        plain+=k
    # Hậu xử lý
    i = 0
    s = ""
    for x in ciphertext:
        if x == ' ':
            s +=" "
            continue
        if x <'a':
            s += plain[i].upper()
        else:
            s += plain[i]
        i+=1
    return s
plaintext = input("PlainText: ")
key = input("Key: ")
cipher = Encryption(plaintext,key)
print("Cipher:", cipher)
print("Plaintext:", Decryption(cipher,key))