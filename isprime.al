disable_internal_display()
set( num, 8 )
set( isprime, true )
set( sqnum, "%{echo \"sqrt(${num}) + 1\" | bc}" )

foreach( x, 2, "${sqnum}" ) {
	set( tmp, "%{echo ${num} % ${x} | bc}" )
	if( "${tmp}" == 0 ) {
		set( isprime, false )
		brkloop()
	}
}

if( "${isprime}" == true ) {
	print( "${num} is prime\n" )
} else {
	print( "${num} is not prime\n" )
}

#enable_internal_display()