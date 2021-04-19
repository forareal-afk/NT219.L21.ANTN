try:
    f = open("test.txt", encoding = 'utf-8')
    b = f.readlines()
    s = ''.join(b).replace('\n',' ')
    f.writelines(s)
    f.close()
finally:
    



