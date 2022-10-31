
#ifndef ABI_FORTRAN_INTEROP_H
#define ABI_FORTRAN_INTEROP_H

/* THIS IS A FORTRAN FILE! */
/* clang-format off */

#if defined(__INTEL_COMPILER) || defined(__IBMC__)
! check for Intel compiler first because it ALSO defines __GNUC__
! We can use # and ## as in the standard C preprocessor

#define STR(x) #x
#define BIND(x) bind(c, name = STR(x))

#define CLASS_WRAP(class_arg) class(class_arg##_t)

#define CREATE_C(package, class)      eap_##package##_##class##_create
#define CREATE_BIND(package, class)   BIND(eap_##package##_##class##_create)

#define FREE(package, class)          package##_##class##_t_free
#define IS_ASSOCIATED(package, class) package##_##class##_t_is_associated

#define DELETE_C(package, class)      eap_##package##_##class##_delete
#define DELETE_BIND(package, class)   BIND(eap_##package##_##class##_delete)

#define GET_FIELD(package, class, field)       package##_##class##_t_get_##field
#define GET_FIELD_COMP(package, class, field)  package##_##class##_t_get_##field##_comp
#define GET_FIELD_C(package, class, field)     eap_##package##_##class##_get_##field
#define GET_FIELD_BIND(package, class, field)  BIND(eap_##package##_##class##_get_##field)

#elif defined(__GNUC__)
! GNU runs the preprocessor in traditional mode for Fortran macros. In traditional mode, the
! operators # and ## are not available. So we need to fake ## with /**/. Unfortunately, the Intel
! Fortran preprocessor replaces /**/ with a space, so that's why there is a different implementation
! of these macros for Intel

#define STR(x) "x"
#define BIND(x) bind(c, name = STR(x))

#define CLASS_WRAP(class_arg) class(class_arg/**/_t)

#define CREATE_C(package, class)      eap_/**/package/**/_/**/class/**/_create
#define CREATE_BIND(package, class)   BIND(eap_/**/package/**/_/**/class/**/_create)

#define FREE(package, class)          package/**/_/**/class/**/_t_free
#define IS_ASSOCIATED(package, class) package/**/_/**/class/**/_t_is_associated

#define DELETE_C(package, class)      eap_/**/package/**/_/**/class/**/_delete
#define DELETE_BIND(package, class)   BIND(eap_/**/package/**/_/**/class/**/_delete)

#define GET_FIELD(package, class, field)       package/**/_/**/class/**/_get_/**/field
#define GET_FIELD_COMP(package, class, field)  package/**/_/**/class/**/_get_/**/field/**/_comp
#define GET_FIELD_C(package, class, field)     eap_/**/package/**/_/**/class/**/_get_/**/field
#define GET_FIELD_BIND(package, class, field)                                                      \
    BIND(eap_/**/package/**/_/**/class/**/_get_/**/field)

#else
#error "Unsupported compiler"
#endif

#define FC_INTEROP_CONSTRUCTOR(package, class)                                                     \
    subroutine CREATE_C(package,class)(class) CREATE_BIND(package,class)                          ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        type(c_ptr), intent(inout) :: class                                                       ;\
    end subroutine

#define FC_INTEROP_DESTRUCTOR(package, class)                                                      \
    subroutine DELETE_C(package,class)(class) DELETE_BIND(package,class)                          ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        type(c_ptr), intent(inout) :: class                                                       ;\
    end subroutine

#define FC_INTEROP_FREE_IMPL(package, class)                                                       \
    subroutine FREE(package,class)(this)                                                          ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        CLASS_WRAP(class), intent(inout) :: this                                                  ;\
        if (this%is_associated()) call DELETE_C(package,class)(this%ptr)                          ;\
        this%ptr = c_null_ptr                                                                     ;\
    end subroutine

#define FC_INTEROP_IS_ASSOCIATED_IMPL(package, class)                                              \
    logical function IS_ASSOCIATED(package,class)(this)                                           ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        CLASS_WRAP(class), intent(in) :: this                                                     ;\
        IS_ASSOCIATED(package,class) = c_associated(this%ptr)                                     ;\
    end function

#define FC_INTEROP_WRAP_SCALAR(package, class, field)                                              \
    subroutine GET_FIELD_C(package,class,field)(class, ptr) GET_FIELD_BIND(package,class,field)   ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        type(c_ptr), intent(in), value :: class                                                   ;\
        type(c_ptr), intent(out) :: ptr                                                           ;\
    end subroutine

#define FC_INTEROP_WRAP_SCALAR_IMPL(package, class, field, type_arg)                               \
    function GET_FIELD(package,class,field)(this)                                                 ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        CLASS_WRAP(class), intent(in) :: this                                                     ;\
        type_arg, pointer :: GET_FIELD(package,class,field)                                       ;\
        type(c_ptr) :: field_ptr                                                                  ;\
        call GET_FIELD_C(package,class,field)(this%ptr, field_ptr)                                ;\
        call c_f_pointer(field_ptr, GET_FIELD(package,class,field))                               ;\
    end function

#define FC_INTEROP_WRAP_ARRAY(package, class, field)                                               \
    subroutine GET_FIELD_C(package,class,field)(class, ptr, n0)                                    \
        GET_FIELD_BIND(package,class,field)                                                       ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        type(c_ptr), intent(in), value :: class                                                   ;\
        type(c_ptr), intent(out) :: ptr                                                           ;\
        integer(c_int64_t), intent(out) :: n0                                                     ;\
    end subroutine

#define FC_INTEROP_WRAP_ARRAY_IMPL(package, class, field, type_arg)                                \
    function GET_FIELD(package,class,field)(this)                                                 ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        CLASS_WRAP(class), intent(in) :: this                                                     ;\
        type_arg, pointer :: GET_FIELD(package,class,field)(:)                                    ;\
        integer(c_int64_t) :: n0                                                                  ;\
        type(c_ptr) :: field_ptr                                                                  ;\
        call GET_FIELD_C(package,class,field)(this%ptr, field_ptr, n0)                            ;\
        call c_f_pointer(field_ptr, GET_FIELD(package,class,field), (/n0/))                       ;\
    end function

#define FC_INTEROP_WRAP_ARRAY_COMPONENT_IMPL(package, class, field, type_arg)                      \
    function GET_FIELD_COMP(package,class,field)(this, component)                                 ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        CLASS_WRAP(class), intent(in) :: this                                                     ;\
        integer, intent(in) :: component                                                          ;\
        type_arg, pointer :: temp(:), GET_FIELD_COMP(package,class,field)                         ;\
        integer(c_int64_t) :: n0                                                                  ;\
        type(c_ptr) :: field_ptr                                                                  ;\
        call GET_FIELD_C(package,class,field)(this%ptr, field_ptr, n0)                            ;\
        call c_f_pointer(field_ptr, temp, (/n0/))                                                 ;\
        GET_FIELD_COMP(package,class,field) => temp(component)                                    ;\
    end function

#define FC_INTEROP_WRAP_1D_VIEW(package, class, field)                                             \
    subroutine GET_FIELD_C(package,class,field)(class, ptr, n0)                                    \
        GET_FIELD_BIND(package,class,field)                                                       ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        type(c_ptr), intent(in), value :: class                                                   ;\
        type(c_ptr), intent(out) :: ptr                                                           ;\
        integer(c_int64_t), intent(out) :: n0                                                     ;\
    end subroutine

#define FC_INTEROP_WRAP_1D_VIEW_IMPL(package, class, field, type_arg)                              \
    function GET_FIELD(package,class,field)(this)                                                 ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        CLASS_WRAP(class), intent(in) :: this                                                     ;\
        type_arg, pointer :: GET_FIELD(package,class,field)(:)                                    ;\
        integer(c_int64_t) :: n0                                                                  ;\
        type(c_ptr) :: field_ptr                                                                  ;\
        call GET_FIELD_C(package,class,field)(this%ptr, field_ptr, n0)                            ;\
        call c_f_pointer(field_ptr, GET_FIELD(package,class,field), (/n0/))                       ;\
    end function

#define FC_INTEROP_WRAP_2D_VIEW(package, class, field)                                             \
    subroutine GET_FIELD_C(package,class,field)(class, ptr, n0, n1)                                \
        GET_FIELD_BIND(package,class,field)                                                       ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        type(c_ptr), intent(in), value :: class                                                   ;\
        type(c_ptr), intent(out) :: ptr                                                           ;\
        integer(c_int64_t), intent(out) :: n0, n1                                                 ;\
    end subroutine

#define FC_INTEROP_WRAP_2D_VIEW_IMPL(package, class, field, type_arg)                              \
    function GET_FIELD(package,class,field)(this)                                                 ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        CLASS_WRAP(class), intent(in) :: this                                                     ;\
        type_arg, pointer :: GET_FIELD(package,class,field)(:,:)                                  ;\
        integer(c_int64_t) :: n0, n1                                                              ;\
        type(c_ptr) :: field_ptr                                                                  ;\
        call GET_FIELD_C(package,class,field)(this%ptr, field_ptr, n0, n1)                        ;\
        call c_f_pointer(field_ptr, GET_FIELD(package,class,field), (/n0, n1/))                   ;\
    end function

#define FC_INTEROP_WRAP_3D_VIEW(package, class, field)                                             \
    subroutine GET_FIELD_C(package,class,field)(class, ptr, n0, n1, n2)                            \
        GET_FIELD_BIND(package,class,field)                                                       ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        type(c_ptr), intent(in), value :: class                                                   ;\
        type(c_ptr), intent(out) :: ptr                                                           ;\
        integer(c_int64_t), intent(out) :: n0, n1, n2                                             ;\
    end subroutine

#define FC_INTEROP_WRAP_3D_VIEW_IMPL(package, class, field, type_arg)                              \
    function GET_FIELD(package,class,field)(this)                                                 ;\
        use iso_c_binding                                                                         ;\
        implicit none                                                                             ;\
        CLASS_WRAP(class), intent(in) :: this                                                     ;\
        type_arg, pointer :: GET_FIELD(package,class,field)(:,:,:)                                ;\
        integer(c_int64_t) :: n0, n1, n2                                                          ;\
        type(c_ptr) :: field_ptr                                                                  ;\
        call GET_FIELD_C(package,class,field)(this%ptr, field_ptr, n0, n1, n2)                    ;\
        call c_f_pointer(field_ptr, GET_FIELD(package,class,field), (/n0, n1, n2/))               ;\
    end function

/* clang-format on */

#endif
