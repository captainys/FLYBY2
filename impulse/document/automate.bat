REM Automatic Manual Creation



REM Japanese version

copy src\func.txt + src\struct.txt + src\macro.txt + src\primrc.txt + impulse2g.txt tmp1.txt

copy tmp1.txt + \src\hotas\document\src\func.txt + \src\hotas\document\src\macro.txt + \src\hotas\document\src\struct.txt  tmp2.txt

copy tmp2.txt + \src\skyhawk\document\func.txt + \src\skyhawk\document\macro.txt tmp3.txt

REM copy tmp3.txt + \src\aurora\document\src\func.txt tmp4.txt
    copy tmp3.txt all1.txt



src\sortblk < all1.txt > all2.txt

src\fixref < all2.txt > all3.txt
src\fixref < all3.txt > all4.txt

src\htmlgen < all4.txt > all5.txt

copy head.html + all5.txt + tail.html referencej.html

del tmp?.txt
del all?.txt




REM English version

copy src\e_func.txt + src\e_struct.txt + src\e_macro.txt + src\e_primrc.txt + e_impulse2g.txt all1.txt




src\sortblk < all1.txt > all2.txt

src\fixref < all2.txt > all3.txt
src\fixref < all3.txt > all4.txt

src\htmlgen < all4.txt > all5.txt

copy head.html + all5.txt + tail.html referencee.html

del tmp?.txt
del all?.txt
