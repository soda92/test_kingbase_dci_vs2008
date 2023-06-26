#ifndef DCIMISC_H
#define DCIMISC_H

sword test_ServerVersion();
sword testConst();
sword testDirPathLoadStream_error();
sword testProcedure();
sword test_multi_execute();
sword test_select_rows();
sword test_precision_scale();
sword test_precision_scale1();
sword test_precision_scale2();
sword test_precision_scale3();
sword test_multiple_sql();
sword test_result_length();
sword test_bug12869();
sword test_dblink();
sword test_PLAN_Exist();
sword test_bug13899();
sword test_bug14055();
sword test_TransStart();
sword test_bug14058();
sword test_bug14243();
sword test_union_all();
sword test_begin_end();
sword test_begin_end2();
sword test_UseDciDat_0();
sword test_UseDciDat_1();
sword test_UseDciDat_2();
sword test_bug14060();
sword test_bug15341();
sword test_BIT();
sword test_FloatBindByInt();
sword test_bug21728(); 
sword test_select_end_colon_r_n(); 
sword test_bug22130();
sword test_bug22490();
sword test_bug22502();
sword test_bug22774();
sword test_bug22775();
sword test_bind_in_anonymous_block();
sword test_bind_in_anonymous_block2();
sword test_bind_out_dml_returning_into();
sword test_bug23012();
sword test_bug23027();
sword test_bug23066();
sword test_bug23073();
sword test_bug24038();
sword test_bug24040();
sword test_bug24041();
sword test_bug23191();
sword test_with_as();
#ifdef WIN32
unsigned __stdcall test_thread(void *arg);
#else
void* test_thread(void *arg);
#endif

#endif /* DCIMISC_H */
