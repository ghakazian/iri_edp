c  irisubc.for Wrapper with ISO_C binding for the iri_web entrypoint
c  This wrapper also performs required steps to read indices data 
c  prior to calling the entrypoint
      subroutine iri_web_c(jmag,jf,alati,along,iyyyy,mmdd,iut,dhour,
     &  height,h_tec_max,ivar,vbeg,vend,vstp,a,b) bind(c, 
     &  name="iri_web_for_c")
        use iso_c_binding
        integer(c_int), intent(in) :: jmag
        logical(c_int), intent(in) :: jf(50)
        real(c_float), intent(in) :: alati
        real(c_float), intent(in) :: along
        integer(c_int), intent(in) :: iyyyy
        integer(c_int), intent(in) :: mmdd
        integer(c_int), intent(in) :: iut
        real(c_float), intent(in) :: dhour
        real(c_float), intent(in) :: height
        real(c_float), intent(in) :: h_tec_max
        integer(c_int), intent(in) :: ivar
        real(c_float), intent(in) :: vbeg
        real(c_float), intent(in) :: vend
        real(c_float), intent(in) :: vstp
        real(c_float), intent(out) :: a(20,1000)
        real(c_float), intent(out) :: b(100,1000)

        call read_ig_rz
        call readapf107
        
        call iri_web(jmag,jf,alati,along,iyyyy,mmdd,iut,dhour,
     &          height,h_tec_max,ivar,vbeg,vend,vstp,a,b)
      end 