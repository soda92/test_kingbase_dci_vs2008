#include "DciTest.h"
#include "errcode.h"

sword test_errcode_00001()
{
	sword	ret = DCI_SUCCESS;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = execute("create table t_00001(c int unique)");
	err = execute("insert into t_00001 values(1)");
	err = execute("insert into t_00001 values(1)");
	if (err == DCI_ERROR)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		if (errcode != 1)
		{
			printf("test_errcode_00001: expected 00001, but return %d\n", errcode);
			ret = DCI_ERROR;
		}
	}
	else
	{
		printf("test_errcode_00001: expected DCI_ERROR, but return %d\n", err);
		ret = DCI_ERROR;
	}

	err = execute("drop table t_00001");

end:
	err = disconnect(true);
	return ret;
}

sword test_errcode_00942()
{
	sword	ret = DCI_SUCCESS;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = execute("select * from t_00942");
	if (err == DCI_ERROR)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		if (errcode != 942)
		{
			printf("test_errcode_00942: expected 00942, but return %d\n", errcode);
			ret = DCI_ERROR;
		}
	}
	else
	{
		printf("test_errcode_00942: expected DCI_ERROR, but return %d\n", err);
		ret = DCI_ERROR;
	}

end:
	err = disconnect(true);
	return ret;
}

sword test_errcode_03114()
{
	sword	ret = DCI_SUCCESS;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = execute("select * from dual");
	if (err == DCI_ERROR)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		err = DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		if (err == DCI_SUCCESS)
		{
			printf("test_errcode_03114: %d %s\n", errcode, msg);
		}
	}
	else
	{
		printf("test_errcode_03114: expected DCI_ERROR, but return %d\n", err);
		ret = DCI_ERROR;
	}

end:
	err = disconnect(true);
	return ret;
}
