;tunnel datas
include texture.tbl
include sincos.tbl
include tunnel.tbl
include tunn1.tbl

PAL1	label byte
	j=0
	rept 22
		db j, j, j
		db j, j, j
		db j, j, j
		j=j+1
	endm

PAL2	label byte
	j=0
	rept 64
		db j, j, j
		j=j+1
	endm

