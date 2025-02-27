import Device_Korad

s1 = Device_Korad.Korad("Korad.ini")
s2 = s1
del s1
print(s2)
