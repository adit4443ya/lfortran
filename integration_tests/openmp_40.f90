program openmp_40
integer :: i, j, res=0
do concurrent ( i =1:4, j = 1:3 ) reduce(+:res)
    print *,i
    res=res+i
end do 
print *,"res =",res
end program