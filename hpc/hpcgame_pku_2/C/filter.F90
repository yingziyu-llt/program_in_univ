module filter_run_module
    use, intrinsic :: iso_c_binding
    implicit none
contains
    subroutine filter_run(x, wgt, ngrid, is, ie, js, je) bind(C, name="filter_run")
        use iso_c_binding, only: c_double, c_int
        implicit none

        ! 参数声明
        integer(c_int), intent(in), value :: is, ie, js, je
        integer(c_int), intent(in) :: ngrid(0:112)
        real(c_double), intent(inout) :: x(0:112, 0:2699)
        real(c_double), intent(in) :: wgt(0:112, 0:1798)

        integer(c_int) :: i, j, p, n, hn
        real(c_double), allocatable :: tmp(:)

        ! 打印参数值以便调试
        print *, "is:", is, " ie:", ie, " js:", js, " je:", je

        ! 循环处理
        do j = js, je
            n = ngrid(j)
            hn = (n - 1) / 2

            ! 分配临时数组大小并初始化为0
            allocate(tmp(is:ie))
            tmp = 0.0d0

            ! 计算卷积
            do i = is, ie
                do p = 0, n - 1
                    tmp(i) = tmp(i) + wgt(j, p) * x(j, i - hn + p)
                end do
            end do

            ! 将临时数组的结果赋值回x
            do i = is, ie
                x(j, i) = tmp(i)
            end do

            ! 释放临时数组内存
            deallocate(tmp)
        end do
    end subroutine filter_run
end module filter_run_module