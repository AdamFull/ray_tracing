.code
sqrt14 PROC
    fld qword ptr [esp+4]
    fsqrt
    ret 8
sqrt14 ENDP
END