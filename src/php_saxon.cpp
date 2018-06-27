#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "php_saxon.h"

#ifdef COMPILE_DL_SAXON
    extern "C" {
        ZEND_GET_MODULE(saxon)
    }
#endif

zend_object_handlers saxonProcessor_object_handlers;
zend_object_handlers xsltProcessor_object_handlers;
zend_object_handlers xqueryProcessor_object_handlers;
zend_object_handlers xpathProcessor_object_handlers;
zend_object_handlers schemaValidator_object_handlers;
zend_object_handlers xdmValue_object_handlers;
zend_object_handlers xdmItem_object_handlers;
zend_object_handlers xdmNode_object_handlers;
zend_object_handlers xdmAtomicValue_object_handlers;

zend_class_entry *saxonProcessor_ce;
zend_class_entry *xsltProcessor_ce;
zend_class_entry *xqueryProcessor_ce;
zend_class_entry *xpathProcessor_ce;
zend_class_entry *schemaValidator_ce;
zend_class_entry *xdmValue_ce;
zend_class_entry *xdmItem_ce;
zend_class_entry *xdmNode_ce;
zend_class_entry *xdmAtomicValue_ce;

void SaxonProcessor_free_storage(void *object TSRMLS_DC)
{
 
    saxonProcessor_object *obj = (saxonProcessor_object *)object;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value saxonProcessor_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    saxonProcessor_object *obj = (saxonProcessor_object *)emalloc(sizeof(saxonProcessor_object));
    memset(obj, 0, sizeof(saxonProcessor_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);
    retval.handle = zend_objects_store_put(obj, NULL, SaxonProcessor_free_storage, NULL TSRMLS_CC);
    retval.handlers = &saxonProcessor_object_handlers;

    return retval;
}

PHP_METHOD(SaxonProcessor, __construct)
{

    if (ZEND_NUM_ARGS()>2) {
        WRONG_PARAM_COUNT;
    }

    char * cwdi = NULL;
   bool license = false;
    int len1;
    if (ZEND_NUM_ARGS()==1 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &license) == FAILURE) {
        php_error(E_WARNING,"Wrong SaxonProcessor argument");
        RETURN_NULL();
    } 


    if (ZEND_NUM_ARGS()>1 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "bs", &license, &cwdi, &len1) == FAILURE) {
        php_error(E_WARNING,"Wrong SaxonProcessor arguments");
        RETURN_NULL();
    }
 

    zval *object = getThis();
    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj = (saxonProcessor_object *)zend_object_store_get_object(object TSRMLS_CC);
    saxonProc =  obj->saxonProcessor;
    if(saxonProc == NULL) {

	saxonProc = new SaxonProcessor(license); //TODO: add license flag to PHP function argument

	obj->saxonProcessor = saxonProc;
    }

    if(cwdi==NULL) {
#if !(defined (__linux__) || (defined (__APPLE__) && defined(__MACH__)))
	    TCHAR s[256];

            // --
            DWORD a = GetCurrentDirectory(256, s);
	    const size_t newsize = wcslen(s)*2;
	    char* cwd = new char[newsize];
	    wcstombs_s(0, cwd, newsize, s, _TRUNCATE);
	    // -- code above returns the apache installtion directory as the CWD

	    char* cwd2;

	    //php_error(E_WARNING,cwd2);

	    saxonProc->setcwd(cwd2);
	    // -- code above tries to use VCWD_GETCWD but there is a linkage error
#else

	    char cwd[256];

	    VCWD_GETCWD(cwd, sizeof(cwd));
	    if(cwd == NULL) {
	     //php_error(E_WARNING,"cwd is null");
	   }else {
             //php_error(E_WARNING,cwd);

	    saxonProc->setcwd(cwd);
          }
#endif

    } else {
        saxonProc->setcwd(cwdi);
    }

}

PHP_METHOD(SaxonProcessor, __destruct)
{
    
    saxonProcessor_object *obj = (saxonProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    SaxonProcessor * saxonProc= obj->saxonProcessor;

    delete saxonProc;
}

PHP_METHOD(SaxonProcessor, setResourcesDirectory)
{
    SaxonProcessor *saxonProcessor;
    char * dirStr;
    int len;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dirStr, &len) == FAILURE) {
        RETURN_NULL();
    }
    
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != NULL) {
       
        if(dirStr != NULL) {
            saxonProcessor->setResourcesDirectory(dirStr);
        }
    }
}




PHP_METHOD(SaxonProcessor, setcwd)
{
    SaxonProcessor *saxonProcessor;
    char * cwdStr;
    int len;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &cwdStr, &len) == FAILURE) {
        RETURN_NULL();
    }
    
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != NULL) {
        
        if(cwdStr != NULL) {
            saxonProcessor->setcwd(cwdStr);
        }
    }
}


PHP_METHOD(SaxonProcessor, parseXmlFromString)
{
    SaxonProcessor * saxonProcessor;
    char * source;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &len1) == FAILURE) {
        RETURN_NULL();
    }
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != NULL);
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != NULL) {
        XdmNode* node = saxonProcessor->parseXmlFromString(source);
        if(node != NULL) {
            if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
                struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmNode = node;
            }
        } else {
            if(obj->saxonProcessor->exceptionOccurred()){
		//TODO throw exception
	    }
        }
    } else {
        RETURN_NULL();
    }
}

PHP_METHOD(SaxonProcessor, parseXmlFromFile)
{
    SaxonProcessor * saxonProcessor;
    char * source;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &len1) == FAILURE) {
        RETURN_NULL();
    }
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != NULL);
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != NULL) {
        XdmValue* node = (XdmValue*)saxonProcessor->parseXmlFromFile(source);//TODO this needs to be XdmNode object
        if(node != NULL) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
                struct xdmValue_object* vobj = (struct xdmValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmValue = node;
            }
        } else {
           // obj->xsltProcessor->checkException();//TODO
        }
    } else {
        RETURN_NULL();
    }
}


PHP_METHOD(SaxonProcessor, createAtomicValue)
{
    XdmAtomicValue * xdmValue = NULL;
    SaxonProcessor * proc;
    char * source;
    int len1;
    zval *zvalue;
    bool bVal;
    char * sVal;
    int len;
    long iVal;
    double dVal;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",&zvalue) == FAILURE) {
        RETURN_NULL();
    }
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != NULL);
    proc = obj->saxonProcessor;
    assert (proc != NULL);
    if (proc != NULL) {
	switch (Z_TYPE_P(zvalue)) {
            case IS_BOOL:
                bVal = Z_BVAL_P(zvalue);
                xdmValue = proc->makeBooleanValue((bool)bVal);
            break;
            case IS_LONG:
                iVal = Z_LVAL_P(zvalue);
		 xdmValue = proc->makeIntegerValue((int)iVal);
            break;
            case IS_STRING:
                sVal = Z_STRVAL_P(zvalue);
                len = Z_STRLEN_P(zvalue);
                xdmValue = proc->makeStringValue((const char*)sVal);
            break;
            case IS_NULL:
                xdmValue = new XdmAtomicValue();
            break;
            case IS_DOUBLE:
                dVal = (double)Z_DVAL_P(zvalue);
		xdmValue = proc->makeDoubleValue((double)iVal);
                break;
            case IS_ARRAY:
                // TODO: Should not be expected. Do this some other way
                //break;
            case IS_OBJECT:
                // TODO: implement this
                //break;
            default:
                obj = NULL;
                zend_throw_exception(zend_exception_get_default(TSRMLS_C), "unknown type specified in XdmValue", 0 TSRMLS_CC); 
                RETURN_NULL();
        }
        if(xdmValue == NULL) {
            RETURN_NULL();
        }
        if (object_init_ex(return_value, xdmAtomicValue_ce) != SUCCESS) {
            RETURN_NULL();
        } else {
            struct xdmAtomicValue_object* vobj = (struct xdmAtomicValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
            assert (vobj != NULL);
            vobj->xdmAtomicValue = xdmValue;
        }
    } else {
       
        RETURN_NULL();
    }
}


PHP_METHOD(SaxonProcessor, newXPathProcessor)
{
   
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    XPathProcessor * xpathProcessor = NULL;
    if((saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC) == NULL) {
      
    }
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    assert (obj != NULL);
    proc = obj->saxonProcessor;
    assert (proc != NULL);
    if (proc != NULL) {
if (object_init_ex(return_value, xpathProcessor_ce) != SUCCESS) {
            RETURN_NULL();
        } else {
	   struct xpathProcessor_object* vobji = (struct xpathProcessor_object *)zend_object_store_get_object(return_value TSRMLS_CC);
            assert (vobji != NULL);
	    xpathProcessor = proc->newXPathProcessor();
            vobji->xpathProcessor = xpathProcessor;
	}
    } else {
       
        RETURN_NULL();
    }
}

PHP_METHOD(SaxonProcessor, newXsltProcessor)
{
    //php_error(E_WARNING,"new xsltProc 1");
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    XsltProcessor * xsltProcessor = NULL;
if((saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC) == NULL) {
   
}

    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != NULL);
    proc = obj->saxonProcessor;
    assert (proc != NULL);
    if (proc != NULL) {
    if (object_init_ex(return_value, xsltProcessor_ce) != SUCCESS) {
            RETURN_NULL();
        } else {
	   struct xsltProcessor_object* vobj = (struct xsltProcessor_object *)zend_object_store_get_object(return_value TSRMLS_CC);
	
            assert (vobj != NULL);
	    xsltProcessor = proc->newXsltProcessor();
            vobj->xsltProcessor = xsltProcessor;
    }
    } else {
       
        RETURN_NULL();
    }
}

PHP_METHOD(SaxonProcessor, newXQueryProcessor)
{
   
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    XQueryProcessor * xqueryProcessor = NULL;
   if((saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC) == NULL) {
   
   }
   
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != NULL);
    proc = obj->saxonProcessor;
    assert (proc != NULL);
    if (proc != NULL) {
	if (object_init_ex(return_value, xqueryProcessor_ce) != SUCCESS) {
            RETURN_NULL();
        } else {
	   struct xqueryProcessor_object* vobj = (struct xqueryProcessor_object *)zend_object_store_get_object(return_value TSRMLS_CC);

            assert (vobj != NULL);
	    xqueryProcessor = proc->newXQueryProcessor();
            vobj->xqueryProcessor = xqueryProcessor;
     }
    } else {
       
        RETURN_NULL();
    }
}

PHP_METHOD(SaxonProcessor, newSchemaValidator)
{
   
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    SchemaValidator * schemaValidator = NULL;
   
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != NULL);
    proc = obj->saxonProcessor;
    assert (proc != NULL);
    if (proc != NULL) {
	if (object_init_ex(return_value, schemaValidator_ce) != SUCCESS) {
            RETURN_NULL();
        } else {
	   struct schemaValidator_object* vobj = (struct schemaValidator_object *)zend_object_store_get_object(return_value TSRMLS_CC);
            assert (vobj != NULL);
	    schemaValidator = proc->newSchemaValidator();
	    if(schemaValidator == NULL){
		RETURN_NULL();
	    }
            vobj->schemaValidator = schemaValidator;
	}
    } else {
       
        RETURN_NULL();
    }
}


PHP_METHOD(SaxonProcessor, version)
{
    SaxonProcessor *saxonProcessor;
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != NULL) {
        char *str = estrdup(saxonProcessor->version());
        RETURN_STRING(str, 0);
    }
    RETURN_NULL();
}


PHP_METHOD(SaxonProcessor, setConfigurationProperty)
{
    SaxonProcessor *saxonProcessor;
    char * name;
    int len1;
    char * value;
    int len2;
    if (ZEND_NUM_ARGS()!= 2) {
        WRONG_PARAM_COUNT;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_NULL();
    }
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != NULL && name != NULL && value != NULL) {
        saxonProcessor->setConfigurationProperty(name, value);
    }
    
}

/*     ============== XSLT10/20/30/31: PHP Interface of   XsltProcessor class =============== */

void XsltProcessor_free_storage(void *object TSRMLS_DC)
{

    xsltProcessor_object *obj = (xsltProcessor_object *)object;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);

}

zend_object_value xsltProcessor_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    xsltProcessor_object *obj = (xsltProcessor_object *)emalloc(sizeof(xsltProcessor_object));
    memset(obj, 0, sizeof(xsltProcessor_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);
    retval.handle = zend_objects_store_put(obj, NULL, XsltProcessor_free_storage, NULL TSRMLS_CC);
    retval.handlers = &xsltProcessor_object_handlers;

    return retval;
}



PHP_METHOD(XsltProcessor, __destruct)
{
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);


    XsltProcessor * xsltProcessor= obj->xsltProcessor;

    delete xsltProcessor;
    
}

PHP_METHOD(XsltProcessor, transformFileToFile)
{
    XsltProcessor *xsltProcessor;
    char * outfileName;
    char * infilename;
    char * styleFileName;
    int len1, len2, len3;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &infilename, &len1, &styleFileName, &len2, &outfileName, &len3) == FAILURE) {
        RETURN_NULL();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
 
    if (xsltProcessor != NULL) {

        xsltProcessor->transformFileToFile(infilename, styleFileName, outfileName);
        if(xsltProcessor->exceptionOccurred()) {
     	  // TODO: throw exception
        }
    }
}

PHP_METHOD(XsltProcessor, transformFileToValue)
{
    XsltProcessor *xsltProcessor;
    char * infilename;
    char * styleFileName;
    int len1, len2;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &infilename, &len1, &styleFileName, &len2) == FAILURE) {
        RETURN_NULL();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    
    if (xsltProcessor != NULL) {

        XdmValue * node = xsltProcessor->transformFileToValue(infilename, styleFileName);
        if(node != NULL) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
                struct xdmValue_object* vobj = (struct xdmValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmValue = node;
            }
        } else {
            if(obj->xsltProcessor->exceptionOccurred()){
  		//TODO
	    }
        }
    }else {
        RETURN_NULL();
    }
}


PHP_METHOD(XsltProcessor, transformFileToString)
{
    XsltProcessor *xsltProcessor;
    char * infilename;
    char * styleFileName;
    int len1, len2;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &infilename, &len1, &styleFileName, &len2) == FAILURE) {
        RETURN_NULL();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != NULL) {

        const char * result = xsltProcessor->transformFileToString(infilename, styleFileName);
	if(result != NULL) {
            char *str = estrdup(result);
            RETURN_STRING(str, 0);
        } else if(xsltProcessor->exceptionOccurred()){
            //TODO: xsltProcessor->checkException();
            const char * errStr = xsltProcessor->getErrorMessage(0);
            if(errStr != NULL) {
                const char * errorCode = xsltProcessor->getErrorCode(0);
                if(errorCode!=NULL) {
                    // TODO: throw exception
                }
            }
        }
    }
}


PHP_METHOD(XsltProcessor, transformToString)
{
    XsltProcessor *xsltProcessor;
 
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != NULL) {

        const char * result = xsltProcessor->transformToString();
        if(result != NULL) {
            char *str = estrdup(result);
            RETURN_STRING(str, 0);
        } else if(xsltProcessor->exceptionOccurred()){
            xsltProcessor->checkException();
            const char * errStr = xsltProcessor->getErrorMessage(0);
            if(errStr != NULL) {
                const char * errorCode = xsltProcessor->getErrorCode(0);
                if(errorCode!=NULL) {
                    // TODO: throw exception
                }
            }
        }
    }
    RETURN_NULL();
}

PHP_METHOD(XsltProcessor, transformToValue)
{
    XsltProcessor *xsltProcessor;

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != NULL) {

	XdmValue * node = xsltProcessor->transformToValue();
        if(node != NULL) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
                struct xdmValue_object* vobj = (struct xdmValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmValue = node;
            }
        } else if(xsltProcessor->exceptionOccurred()){
            xsltProcessor->checkException();
        }
    } else {
        RETURN_NULL();
    }
}

PHP_METHOD(XsltProcessor, transformToFile)
{
    XsltProcessor *xsltProcessor;
 
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != NULL) {

	xsltProcessor->transformToFile();
        if(xsltProcessor->exceptionOccurred()) {
           //TODO
            const char * exStr = xsltProcessor->checkException();
        }
    } else {
        RETURN_NULL();
    }
}

PHP_METHOD(XsltProcessor, compileFromFile)
{
    XsltProcessor *xsltProcessor;
    char * name;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {
        xsltProcessor->compileFromFile(name);
    }
}

PHP_METHOD(XsltProcessor, compileFromString)
{
    XsltProcessor *xsltProcessor;
    char * stylesheetStr;
    int len1, myint;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &stylesheetStr, &len1) == FAILURE) {
        RETURN_NULL();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {
        xsltProcessor->compileFromString(stylesheetStr);
    }
}

PHP_METHOD(XsltProcessor, compileFromValue)
{
    XsltProcessor *xsltProcessor;
   zval* oth;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &oth, xdmValue_ce) == FAILURE) {
        RETURN_NULL();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {
 	xdmValue_object* ooth = (xdmValue_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmValue * value = ooth->xdmValue;
            if(value != NULL && value->size() == 1 && (value->getHead())->getType() == 3) {
        	xsltProcessor->compileFromXdmNode((XdmNode*)(value->getHead()));
	    }
	}
    }
}




PHP_METHOD(XsltProcessor, setSourceFromXdmValue)
{
    XsltProcessor *xsltProcessor;
    zval* oth = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &oth) == FAILURE) {
        RETURN_NULL();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {

    if(!oth) {
	php_error(E_WARNING, "Error setting source value");
	return;
    } else {

      const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmNode * value = ooth->xdmNode;
            if(value != NULL) {	
		XdmItem  *valueX = (XdmItem*)value;
	        xsltProcessor->setSourceFromXdmValue(valueX);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0) {
	xdmValue_object* ooth = (xdmValue_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmValue * value = ooth->xdmValue;
            if(value != NULL) {
	        xsltProcessor->setSourceFromXdmValue((XdmItem*)value);
            }
        }
      }  

        
    }
  }
}

PHP_METHOD(XsltProcessor, setOutputFile)
{
    XsltProcessor *xsltProcessor;
    char * outputFilename;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &outputFilename, &len1, xdmValue_ce) == FAILURE) {
        RETURN_NULL();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL && outputFilename != NULL) {
        
	 xsltProcessor->setOutputFile(outputFilename);
            
        
    }
}

PHP_METHOD(XsltProcessor, setSourceFromFile)
{
    XsltProcessor *xsltProcessor;
    char * inFilename;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &inFilename, &len1, xdmValue_ce) == FAILURE) {
        RETURN_NULL();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL && inFilename != NULL) {
        
	 xsltProcessor->setSourceFromFile(inFilename);
            
        
    }
}


PHP_METHOD(XsltProcessor, setProperty)
{
    XsltProcessor *xsltProcessor;
    char * name;
    char * value;
    int len1, len2, myint;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_NULL();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {
        xsltProcessor->setProperty(name, value);
    }
}

PHP_METHOD(XsltProcessor, setParameter)
{

   XsltProcessor *xsltProcessor;
   char * name;
   zval* oth;
   int len1, len2, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_NULL();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {


      const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmNode * value = ooth->xdmNode;
            if(value != NULL) {	
	        xsltProcessor->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	xdmValue_object* ooth = (xdmValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmValue * value = ooth->xdmValue;
            if(value != NULL) {
		
                xsltProcessor->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != NULL) {
		
                xsltProcessor->setParameter(name, (XdmValue *)value);
            }
        }



      }

    }
}

PHP_METHOD(XsltProcessor, clearParameters)
{
    XsltProcessor *xsltProcessor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {
        xsltProcessor->clearParameters(true);
    }
}

PHP_METHOD(XsltProcessor, clearProperties)
{
    XsltProcessor *xsltProcessor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {
        xsltProcessor->clearProperties();
    }
}

PHP_METHOD(XsltProcessor, exceptionOccurred)
{
    XsltProcessor *xsltProcessor;
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {
        bool result = xsltProcessor->exceptionOccurred();
        RETURN_BOOL(result);
    }
    RETURN_BOOL(false);
}

PHP_METHOD(XsltProcessor, getExceptionCount)
{
    XsltProcessor *xsltProcessor;
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {
        int count = xsltProcessor->exceptionCount();
        RETURN_LONG(count);
    }
    RETURN_LONG(0);
}

PHP_METHOD(XsltProcessor, getErrorCode)
{
    XsltProcessor *xsltProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {
        const char * errCode = xsltProcessor->getErrorCode((int)index);
        if(errCode != NULL) {
            char *str = estrdup(errCode);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(XsltProcessor, getErrorMessage)
{
    XsltProcessor *xsltProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {
        const char * errStr = xsltProcessor->getErrorMessage((int)index);
        if(errStr != NULL) {
            char *str = estrdup(errStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_NULL();
}
PHP_METHOD(XsltProcessor, exceptionClear)
{
    XsltProcessor *xsltProcessor;
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != NULL) {
        xsltProcessor->exceptionClear();
    }
}




/*     ============== XQuery10/30/31: PHP Interface of   XQueryProcessor =============== */

void xqueryProcessor_free_storage(void *object TSRMLS_DC)
{
    xqueryProcessor_object *obj = (xqueryProcessor_object *)object;
    //obj->xqueryProcessor->clearParameters(true);
    //obj->xqueryProcessor->clearProperties();
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value xqueryProcessor_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xqueryProcessor_object *obj = (xqueryProcessor_object *)emalloc(sizeof(xqueryProcessor_object));
    memset(obj, 0, sizeof(xqueryProcessor_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);
    retval.handle = zend_objects_store_put(obj, NULL, xqueryProcessor_free_storage, NULL TSRMLS_CC);
    retval.handlers = &xqueryProcessor_object_handlers;

    return retval;
}

PHP_METHOD(XQueryProcessor, __destruct)
{
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);


    XQueryProcessor * xqueryProcessor= obj->xqueryProcessor;

    delete xqueryProcessor;
    
}


PHP_METHOD(XQueryProcessor, runQueryToValue)
{
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor = obj->xqueryProcessor;

    if (xqueryProcessor != NULL) {
        XdmValue * node = xqueryProcessor->runQueryToValue();
        if(node != NULL) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                struct xdmValue_object* vobj = (struct xdmValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmValue = node;
                return;
            }
        }
        xqueryProcessor->checkException();//TODO
    } else {
        RETURN_NULL();
    }
}

PHP_METHOD(XQueryProcessor, runQueryToString)
{
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor = obj->xqueryProcessor;

    if (xqueryProcessor != NULL) {
        const char * result = xqueryProcessor->runQueryToString();
        if(result != NULL) {
            char *str = estrdup(result);
            RETURN_STRING(str, 0);
	    return;
        } else {
          xqueryProcessor->checkException(); //TODO
	}
    }
   RETURN_NULL();
}

PHP_METHOD(XQueryProcessor, runQueryToFile)
{

     char * ofilename;
     int len1 =0;
    if (ZEND_NUM_ARGS()!= 1) {
        WRONG_PARAM_COUNT;
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ofilename, &len1) == FAILURE) {
        RETURN_NULL();
    }
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);



    xqueryProcessor = obj->xqueryProcessor;

    if (xqueryProcessor != NULL) {
	if(ofilename != NULL) {
		xqueryProcessor->setOutputFile(ofilename);	
	}
        xqueryProcessor->runQueryToFile(); 
    }

}

PHP_METHOD(XQueryProcessor, setQueryContent)
{
    char * queryStr;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &queryStr, &len1) == FAILURE) {
        RETURN_NULL();
    }
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC); 
    if(queryStr != NULL) { 
      obj->xqueryProcessor->setProperty("qs", queryStr);
   }
}

PHP_METHOD(XQueryProcessor, setQueryFile)
{
   char * fileName;
   int len1;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fileName, &len1) == FAILURE) {
        RETURN_NULL();
    }
    if(fileName != NULL) {
    	xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xqueryProcessor = obj->xqueryProcessor;
    	xqueryProcessor->setQueryFile(fileName);
    }
    	
}

PHP_METHOD(XQueryProcessor, setQueryBaseURI)
{
   char * base;
   int len1;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &base, &len1) == FAILURE) {
        RETURN_NULL();
    }
    if(base != NULL) {
    	xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xqueryProcessor = obj->xqueryProcessor;
    	xqueryProcessor->setQueryBaseURI(base);
    }
    	
}

PHP_METHOD(XQueryProcessor, declareNamespace)
{
   char * prefix;
   char * ns;
   int len1, len2;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &prefix, &len1, &ns, &len2) == FAILURE) {
        RETURN_NULL();
    }
    if(prefix != NULL && ns != NULL) {
    	xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xqueryProcessor = obj->xqueryProcessor;
    	xqueryProcessor->declareNamespace(prefix, ns);
    }
    	
}



PHP_METHOD(XQueryProcessor, setContextItem)
{
   char * context;
   int len1;
   zval* oth;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &oth) == FAILURE) {
        RETURN_NULL();
    }
    if(oth != NULL) {
    	xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xqueryProcessor = obj->xqueryProcessor;
    const char * objName =Z_OBJCE_P(oth)->name;
      //std::cerr<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmNode * value = ooth->xdmNode;
            if(value != NULL) {	
	        xqueryProcessor->setContextItem((XdmItem *)value);
	        return;
            }
        }
      } else if(strcmp(objName, "Saxon\\XdmItem")==0){
	xdmItem_object* ooth = (xdmItem_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmItem * value = ooth->xdmItem;
	    if(value != NULL) {	
                xqueryProcessor->setContextItem(value);
		return;
	    }
         }
        



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != NULL) {
		
                xqueryProcessor->setContextItem((XdmItem *)value);
		return;
            }
        }



      } 




	/*xdmItem_object* ooth = (xdmItem_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmItem * value = ooth->xdmItem;
            if(value != NULL) {
    		xqueryProcessor->setContextItem(value);
	    }
	}*/
    }
	//throw exception
	php_error(E_WARNING,"contextItem not set");
	
    	
}

PHP_METHOD(XQueryProcessor, setContextItemFromFile)
{
   char * cfilename;
   int len1;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &cfilename, &len1) == FAILURE) {
        RETURN_NULL();
    }
    if(cfilename != NULL) {
    	xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xqueryProcessor = obj->xqueryProcessor;
    	xqueryProcessor->setContextItemFromFile(cfilename);
    }
    	
}


PHP_METHOD(XQueryProcessor, setProperty)
{
    XQueryProcessor *xqueryProcessor;
    char * name;
    char * value;
    int len1, len2, myint;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_NULL();
    }
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != NULL) {
        xqueryProcessor->setProperty(name, value);
    }
}

PHP_METHOD(XQueryProcessor, setParameter)
{

   XQueryProcessor *xqueryProcessor;
   char * name;
   zval* oth;
   int len1, len2, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_NULL();
    }
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != NULL) {
             const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmNode * value = ooth->xdmNode;
            if(value != NULL) {	
	        xqueryProcessor->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	xdmValue_object* ooth = (xdmValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmValue * value = ooth->xdmValue;
            if(value != NULL) {
		
                xqueryProcessor->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != NULL) {
		
                xqueryProcessor->setParameter(name, (XdmValue *)value);
            } 
        }



      }

    }
}




PHP_METHOD(XQueryProcessor, clearParameters)
{
    XQueryProcessor *xqueryProcessor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != NULL) {
        xqueryProcessor->clearParameters(true);
    }
}

PHP_METHOD(XQueryProcessor, clearProperties)
{
    XQueryProcessor *xqueryProcessor;

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != NULL) {
        xqueryProcessor->clearProperties();
    }
}

PHP_METHOD(XQueryProcessor, exceptionOccurred)
{
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != NULL) {
        bool result = xqueryProcessor->exceptionOccurred();
        RETURN_BOOL(result);
    }
    RETURN_BOOL(false);
}

PHP_METHOD(XQueryProcessor, getExceptionCount)
{
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != NULL) {
        int count = xqueryProcessor->exceptionCount();
        RETURN_LONG(count);
    }
    RETURN_LONG(0);
}

PHP_METHOD(XQueryProcessor, getErrorCode)
{
    XQueryProcessor *xqueryProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != NULL) {
        const char * errCode = xqueryProcessor->getErrorCode((int)index);
        if(errCode != NULL) {
            char *str = estrdup(errCode);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(XQueryProcessor, getErrorMessage)
{
    XQueryProcessor *xqueryProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    xqueryProcessor_object *obj = (xqueryProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != NULL) {
        const char * errStr = xqueryProcessor->getErrorMessage((int)index);
        if(errStr != NULL) {
            char *str = estrdup(errStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_NULL();
}
PHP_METHOD(XQueryProcessor, exceptionClear)
{
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != NULL) {
        xqueryProcessor->exceptionClear();
    }
}

/*     ============== PHP Interface of XPath2.0/3.0  XPathProcessor =============== */

void xpathProcessor_free_storage(void *object TSRMLS_DC)
{
    xpathProcessor_object *obj = (xpathProcessor_object *)object;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

PHP_METHOD(XPathProcessor, __destruct)
{
   xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
   XPathProcessor * xpathProc= obj->xpathProcessor;


    delete xpathProc;
    
}

zend_object_value xpathProcessor_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xpathProcessor_object *obj = (xpathProcessor_object *)emalloc(sizeof(xpathProcessor_object));
    memset(obj, 0, sizeof(xpathProcessor_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, NULL, xpathProcessor_free_storage, NULL TSRMLS_CC);
    retval.handlers = &xpathProcessor_object_handlers;

    return retval;
}



PHP_METHOD(XPathProcessor, setProperty)
{
    XPathProcessor *xpathProcessor;
    char * name;
    char * value;
    int len1, len2;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_NULL();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
        xpathProcessor->setProperty(name, value);
    }
}

PHP_METHOD(XPathProcessor, setParameter)
{

   XPathProcessor *xpathProcessor;
   char * name;
   zval* oth;
   int len1, len2;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_NULL();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
            const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmNode * value = ooth->xdmNode;
            if(value != NULL) {	
	        xpathProcessor->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	xdmValue_object* ooth = (xdmValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmValue * value = ooth->xdmValue;
            if(value != NULL) {
		
                xpathProcessor->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != NULL) {
		
                xpathProcessor->setParameter(name, (XdmValue *)value);
            }
        }



      }

    }
}

PHP_METHOD(XPathProcessor, declareNamespace)
{
   char * prefix;
   char * ns;
   int len1, len2;
   XPathProcessor *xpathProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &prefix, &len1, &ns, &len2) == FAILURE) {
        RETURN_NULL();
    }
    if(prefix != NULL && ns != NULL) {
    	xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xpathProcessor = obj->xpathProcessor;
    	xpathProcessor->declareNamespace(prefix, ns);
    }
    	
}

PHP_METHOD(XPathProcessor, effectiveBooleanValue)
{

   XPathProcessor *xpathProcessor;
   char * xpathStr;
   zval* oth;
   int len1, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &xpathStr, &len1, xdmValue_ce) == FAILURE) {
        RETURN_NULL();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL && xpathStr != NULL) {
        
                bool result = xpathProcessor->effectiveBooleanValue(xpathStr);
		RETURN_BOOL(result);
    }
}

PHP_METHOD(XPathProcessor, evaluate)
{

   XPathProcessor *xpathProcessor;
   char * xpathStr;
   zval* oth;
   int len1, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &xpathStr, &len1, xdmValue_ce) == FAILURE) {
        RETURN_NULL();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL && xpathStr != NULL) {
        
        XdmValue * node = xpathProcessor->evaluate(xpathStr);
	if(node != NULL) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                struct xdmValue_object* vobj = (struct xdmValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmValue = node;
                return;
            }
        }
        xpathProcessor->checkException();//TODO
    } 
    RETURN_NULL();
    
}

PHP_METHOD(XPathProcessor, evaluateSingle)
{

   XPathProcessor *xpathProcessor;
   char * xpathStr;
   zval* oth;
   int len1, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &xpathStr, &len1, xdmValue_ce) == FAILURE) {
        RETURN_NULL();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;

    if(xpathStr == NULL) { 
	RETURN_NULL();	
	return;
	}


    if (xpathProcessor != NULL) {
        
        XdmItem * node = xpathProcessor->evaluateSingle(xpathStr);
	if(node != NULL) {
            if (object_init_ex(return_value, xdmItem_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                struct xdmItem_object* vobj = (struct xdmItem_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmItem = node;
                return;
            }
        } 
        xpathProcessor->checkException();//TODO
    } 
    RETURN_NULL();
}

PHP_METHOD(XPathProcessor, setContextItem)
{

   XPathProcessor *xpathProcessor;

   zval* oth;
	//TODO this should be relaxed to accept item/atomic/node as well as Value

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &oth) == FAILURE) {
        RETURN_NULL();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
	if(!oth) {
		php_error(E_WARNING, "Error setting source value");
		return;
         } 
        const char * objName =Z_OBJCE_P(oth)->name;
        xdmItem_object* ooth = (xdmItem_object*) zend_object_store_get_object(oth TSRMLS_CC);
       if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmNode * value = ooth->xdmNode;
            if(value != NULL) {	
	        xpathProcessor->setContextItem((XdmItem *)value);
		value->incrementRefCount();

            }
        }
      }  else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != NULL) {
		
                xpathProcessor->setContextItem((XdmItem *)value);
		value->incrementRefCount();
            }
        }
     }   else if(strcmp(objName, "Saxon\\XdmItem")==0){
	xdmItem_object* ooth = (xdmItem_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmItem * value = ooth->xdmItem;
            if(value != NULL) {
		
                xpathProcessor->setContextItem(value);
		value->incrementRefCount();
            }
        }

      }
    }
}

PHP_METHOD(XPathProcessor, setBaseURI)
{

   XPathProcessor *xpathProcessor;

   char * uriStr;
   int len1;
	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &uriStr, &len1, xdmValue_ce) == FAILURE) {
        RETURN_NULL();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
        
        if(uriStr != NULL) {
           
                xpathProcessor->setBaseURI(uriStr);
            
        }
    }
}

PHP_METHOD(XPathProcessor, setContextFile)
{

   XPathProcessor *xpathProcessor;

   char * name;
   int len1;
	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1, xdmValue_ce) == FAILURE) {
        RETURN_NULL();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
        
        if(name != NULL) {
           
                xpathProcessor->setContextFile(name);
            
        }
    }
}

PHP_METHOD(XPathProcessor, clearParameters)
{
    XPathProcessor *xpathProcessor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
        xpathProcessor->clearParameters(true);
    }
}

PHP_METHOD(XPathProcessor, clearProperties)
{
     XPathProcessor *xpathProcessor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
        xpathProcessor->clearProperties();
    }
}


PHP_METHOD(XPathProcessor, exceptionOccurred)
{
   XPathProcessor *xpathProcessor;
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
        bool result = xpathProcessor->exceptionOccurred();
        RETURN_BOOL(result);
    }
    RETURN_BOOL(false);
}

PHP_METHOD(XPathProcessor, getExceptionCount)
{
    XPathProcessor *xpathProcessor;
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
        int count = xpathProcessor->exceptionCount();
        RETURN_LONG(count);
    }
    RETURN_LONG(0);
}

PHP_METHOD(XPathProcessor, getErrorCode)
{
    XPathProcessor *xpathProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
        const char * errCode = xpathProcessor->getErrorCode((int)index);
        if(errCode != NULL) {
            char *str = estrdup(errCode);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(XPathProcessor, getErrorMessage)
{
    XPathProcessor *xpathProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
        const char * errStr = xpathProcessor->getErrorMessage((int)index);
        if(errStr != NULL) {
            char *str = estrdup(errStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_NULL();
}
PHP_METHOD(XPathProcessor, exceptionClear)
{
    XPathProcessor *xpathProcessor;
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != NULL) {
        xpathProcessor->exceptionClear();
    }
}

/*     ============== PHP Interface of   SchemaValidator =============== */

void schemaValidator_free_storage(void *object TSRMLS_DC)
{
    schemaValidator_object *obj = (schemaValidator_object *)object;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value schemaValidator_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    schemaValidator_object *obj = (schemaValidator_object *)emalloc(sizeof(schemaValidator_object));
    memset(obj, 0, sizeof(schemaValidator_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, NULL, schemaValidator_free_storage, NULL TSRMLS_CC);
    retval.handlers = &schemaValidator_object_handlers;

    return retval;
}



PHP_METHOD(SchemaValidator, __destruct)
{
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);


    SchemaValidator * schemaValidator= obj->schemaValidator;

    delete schemaValidator;
    
}



PHP_METHOD(SchemaValidator, registerSchemaFromFile)
{
    SchemaValidator *schemaValidator;
    char * name;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (name != NULL && schemaValidator != NULL) {
        schemaValidator->registerSchemaFromFile(name);
    }
}

PHP_METHOD(SchemaValidator, registerSchemaFromString)
{
    char * schemaStr;
    int len1;
    SchemaValidator *schemaValidator;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &schemaStr, &len1) == FAILURE) {
        RETURN_NULL();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaStr!= NULL && schemaValidator != NULL) {
        schemaValidator->registerSchemaFromString(schemaStr);
    }
}

PHP_METHOD(SchemaValidator, validate)
{
    char * name = NULL;
    int len1;
    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>1) {
        WRONG_PARAM_COUNT;
    }
    if (ZEND_NUM_ARGS()>0 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
        schemaValidator->validate(name);
    }
}

PHP_METHOD(SchemaValidator, validateToNode)
{
    char * name = NULL;
    int len1;
    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>1) {
        WRONG_PARAM_COUNT;
    }
    if (ZEND_NUM_ARGS()>0 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
        XdmNode * node = schemaValidator->validateToNode(name);
	if(node != NULL) {
	    if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmNode = node;
                return;
            }
	} 
    	schemaValidator->checkException();//TODO
    } 
    RETURN_NULL();
}


PHP_METHOD(SchemaValidator, getValidationReport)
{

    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
        XdmNode * node = schemaValidator->getValidationReport();
	if(node != NULL) {
	    if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmNode = node;
                return;
            }
	} 
    	schemaValidator->checkException();//TODO
    } 
    RETURN_NULL();
}


PHP_METHOD(SchemaValidator, setSourceNode)
{
    SchemaValidator *schemaValidator;

    zval* oth;
   

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &oth) == FAILURE) {
        RETURN_NULL();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
	const char * objName =Z_OBJCE_P(oth)->name;
	if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmNode * value = ooth->xdmNode;
            if(value != NULL) {	
		schemaValidator->setSourceNode(value);

            }
        }
      
        
      }
    }
}

PHP_METHOD(SchemaValidator, setOutputFile)
{
    SchemaValidator *schemaValidator;
    char * name;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
        schemaValidator->setOutputFile(name);
    }
}


PHP_METHOD(SchemaValidator, setProperty)
{
    SchemaValidator *schemaValidator;
    char * name;
    char * value;
    int len1, len2, myint;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_NULL();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
        schemaValidator->setProperty(name, value);
    }
}

PHP_METHOD(SchemaValidator, setParameter)
{

   SchemaValidator *schemaValidator;
   char * name;
   zval* oth;
   int len1, len2, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_NULL();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
      const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmNode * value = ooth->xdmNode;
            if(value != NULL) {	
	        schemaValidator->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	xdmValue_object* ooth = (xdmValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmValue * value = ooth->xdmValue;
            if(value != NULL) {
		
                schemaValidator->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != NULL) {
		
                schemaValidator->setParameter(name, (XdmValue *)value);
            }
        }



      }

    }
}

PHP_METHOD(SchemaValidator, clearProperties)
{
    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
        schemaValidator->clearProperties();
	schemaValidator->exceptionClear();
    }
}

PHP_METHOD(SchemaValidator, clearParameters)
{

   SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
       
                schemaValidator->clearParameters(true);
		schemaValidator->exceptionClear();
        }
    }

PHP_METHOD(SchemaValidator, exceptionOccurred)
{
    SchemaValidator *schemaValidator;
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
        bool result = schemaValidator->exceptionOccurred();
        RETURN_BOOL(result);
    }
    RETURN_BOOL(false);
}

PHP_METHOD(SchemaValidator, getExceptionCount)
{
    SchemaValidator *schemaValidator;
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
        int count = schemaValidator->exceptionCount();
        RETURN_LONG(count);
    }
    RETURN_LONG(0);
}

PHP_METHOD(SchemaValidator, getErrorCode)
{
    SchemaValidator *schemaValidator;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
        const char * errCode = schemaValidator->getErrorCode((int)index);
        if(errCode != NULL) {
            char *str = estrdup(errCode);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(SchemaValidator, getErrorMessage)
{
    SchemaValidator *schemaValidator;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    schemaValidator_object *obj = (schemaValidator_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
        const char * errStr = schemaValidator->getErrorMessage((int)index);
        if(errStr != NULL) {
            char *str = estrdup(errStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_NULL();
}
PHP_METHOD(SchemaValidator, exceptionClear)
{
    SchemaValidator * schemaValidator;
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != NULL) {
        schemaValidator->exceptionClear();
    }
}

/*     ============== PHP Interface of   XdmValue =============== */
void xdmValue_free_storage(void *object TSRMLS_DC)
{
    xdmValue_object *obj = (xdmValue_object *)object;
    //obj->xdmValue->releaseXdmValue(NULL);
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value xdmValue_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xdmValue_object *obj = (xdmValue_object *)emalloc(sizeof(xdmValue_object));
    memset(obj, 0, sizeof(xdmValue_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, NULL, xdmValue_free_storage, NULL TSRMLS_CC);
    retval.handlers = &xdmValue_object_handlers;

    return retval;
}

PHP_METHOD(XdmValue, __construct)
{
    XdmValue *xdmValue = NULL;
    bool bVal;
    char * sVal;
    int len;
    long iVal;
    double dVal;
    zval *zvalue;

    SaxonProcessor *proc= NULL;
    xdmValue_object *obj = (xdmValue_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    /*if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",&zvalue) == SUCCESS) {
        switch (Z_TYPE_P(zvalue)) {
            case IS_BOOL:
                bVal = Z_BVAL_P(zvalue);
                xdmValue = new XdmValue(bVal);
                obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
                obj->xdmValue = xdmValue;
            break;
            case IS_LONG:
                iVal = Z_LVAL_P(zvalue);
                xdmValue = new XdmValue((int)iVal);
                obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
                obj->xdmValue = xdmValue;
            break;
            case IS_STRING:
                sVal = Z_STRVAL_P(zvalue);
                len = Z_STRLEN_P(zvalue);
                xdmValue = new XdmValue("string", sVal);
                obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
                obj->xdmValue = xdmValue;
            break;
            case IS_NULL:
                xdmValue = new XdmValue();
                obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
                obj->xdmValue = xdmValue;
            break;
            case IS_DOUBLE:
                // TODO: implement this
                //index = (long)Z_DVAL_P(zvalue);
            //break;
            case IS_ARRAY:
            //break;
            case IS_OBJECT:
            //break;
            default:
                obj = NULL;
                zend_throw_exception(zend_exception_get_default(TSRMLS_C), "unknown type specified in XdmValue", 0 TSRMLS_CC);
        }
    }*/
}

PHP_METHOD(XdmValue, __destruct)
{
    xdmValue_object *obj = (xdmValue_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    XdmValue * xdmValue= obj->xdmValue;
    xdmValue->decrementRefCount();
    if(xdmValue->getRefCount()< 1){
    	delete xdmValue;
    } 
    
    
}

PHP_METHOD(XdmValue,  getHead){
    XdmValue *xdmValue;
    xdmValue_object *obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmValue = obj->xdmValue;
    if (xdmValue != NULL) {
	XdmItem * item = xdmValue->getHead();
	if(item != NULL) {
            if (object_init_ex(return_value, xdmItem_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                struct xdmItem_object* vobj = (struct xdmItem_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmItem = item;
                return;
            }
        }
        
    } else {
	RETURN_NULL();
    }
}


PHP_METHOD(XdmValue,  itemAt){
    XdmValue *xdmValue;

    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_NULL();
    }

    xdmValue_object *obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmValue = obj->xdmValue;
    if (xdmValue != NULL) {
	XdmItem * item = xdmValue->itemAt((int)index);
	if(item != NULL) {
            if (object_init_ex(return_value, xdmItem_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
		item->incrementRefCount();
                struct xdmItem_object* vobj = (struct xdmItem_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmItem = item;
                return;
            }
        }
        
    } else {
	RETURN_NULL();
    }
}


PHP_METHOD(XdmValue,  size){
    XdmValue *xdmValue;
    xdmValue_object *obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmValue = obj->xdmValue;
    int sizei = 0;
    if (xdmValue != NULL) {
	sizei = xdmValue->size();
    }
     RETURN_LONG(sizei);
}


PHP_METHOD(XdmValue, addXdmItem){
    XdmValue *xdmValue;
    zval* oth;
   	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &oth) == FAILURE) {
        RETURN_NULL();
    }

    xdmValue_object *obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmValue = obj->xdmValue;
    if (xdmValue != NULL) {
     const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmNode * value = ooth->xdmNode;
            if(value != NULL) {	
	        xdmValue->addXdmItem((XdmItem *)value);
		return;
            }
        }
      } else if(strcmp(objName, "Saxon\\XdmItem")==0){
	xdmItem_object* ooth = (xdmItem_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmItem * value = ooth->xdmItem;
            if(value != NULL) {
		xdmValue->addXdmItem(value);
		return;
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != NULL) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != NULL) {
		xdmValue->addXdmItem((XdmItem *)value);
		return;
            }
        }

      } else {
		//TODO exception
	}
    }
}



/*     ============== PHP Interface of   XdmItem =============== */

void xdmItem_free_storage(void *object TSRMLS_DC)
{
    xdmItem_object *obj = (xdmItem_object *)object;
    //obj->xdmItem->releaseXdmValue(NULL);
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value xdmItem_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xdmItem_object *obj = (xdmItem_object *)emalloc(sizeof(xdmItem_object));
    memset(obj, 0, sizeof(xdmItem_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, NULL, xdmItem_free_storage, NULL TSRMLS_CC);
    retval.handlers = &xdmItem_object_handlers;

    return retval;
}

PHP_METHOD(XdmItem, __construct)
{
    XdmItem *xdmItem = NULL;
    bool bVal;
    char * sVal;
    int len;
    long iVal;
    double dVal;
    zval *zvalue;

    SaxonProcessor *proc= NULL;
    xdmItem_object *obj = (xdmItem_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

}

PHP_METHOD(XdmItem, __destruct)
{
    xdmItem_object *obj = (xdmItem_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    XdmItem * xdmItem= obj->xdmItem;
    xdmItem->decrementRefCount();
    if(xdmItem->getRefCount()< 1){
    	delete xdmItem;
    }
    
}

PHP_METHOD(XdmItem, getStringValue)
{
    XdmItem *xdmItem;
    xdmItem_object *obj = (xdmItem_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmItem = obj->xdmItem;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;

    if (xdmItem != NULL) {
        const char * valueStr = saxonProc->getStringValue(xdmItem);
        if(valueStr != NULL) {
            char *str = estrdup(valueStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(XdmItem, isAtomic)
{
    XdmItem *xdmItem;
    xdmItem_object *obj = (xdmItem_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmItem = obj->xdmItem;

    if (xdmItem != NULL) {
        bool isAtomic = xdmItem->isAtomic();
        RETURN_BOOL(isAtomic);
    }
    RETURN_BOOL(false);
}

PHP_METHOD(XdmItem, isNode)
{
    XdmItem *xdmItem;
    xdmItem_object *obj = (xdmItem_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmItem = obj->xdmItem;

    if (xdmItem != NULL && xdmItem->getType() == XDM_NODE) {
        RETURN_TRUE;
    }
    RETURN_FALSE;
}

PHP_METHOD(XdmItem, getAtomicValue)
{
    XdmItem *xdmItem;
    xdmItem_object *obj = (xdmItem_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmItem = obj->xdmItem;

    if (xdmItem != NULL) {
	  if(!xdmItem->isAtomic()) {
		RETURN_NULL();
		return;
	  }
          if (object_init_ex(return_value, xdmAtomicValue_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
		xdmItem->incrementRefCount();
                struct xdmAtomicValue_object* vobj = (struct xdmAtomicValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmAtomicValue = (XdmAtomicValue *)xdmItem;
                return;
            }
    }
    RETURN_NULL();
}

PHP_METHOD(XdmItem, getNodeValue)
{
    XdmItem *xdmItem;
    xdmItem_object *obj = (xdmItem_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmItem = obj->xdmItem;

    if (xdmItem != NULL) {
	  if(xdmItem->isAtomic()) {
		RETURN_NULL();
		return;
	  }
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != NULL);
                vobj->xdmNode = (XdmNode *)xdmItem;
		vobj->xdmNode->incrementRefCount();

                return;
            }
    }
    RETURN_NULL();
}



/*     ============== PHP Interface of   XdmNode =============== */

void xdmNode_free_storage(void *object TSRMLS_DC)
{
    xdmNode_object *obj = (xdmNode_object *)object;
    //obj->xdmItem->releaseXdmValue(NULL);
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value xdmNode_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xdmNode_object *obj = (xdmNode_object *)emalloc(sizeof(xdmNode_object));
    memset(obj, 0, sizeof(xdmNode_object));
   // type->name ="XdmNode";
	 
    obj->std.ce = type;	

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, NULL, xdmNode_free_storage, NULL TSRMLS_CC);
    retval.handlers = &xdmNode_object_handlers;

    return retval;
}

PHP_METHOD(XdmNode, __construct)
{
    XdmNode *xdmNode = NULL;
    bool bVal;
    char * sVal;
    int len;
    long iVal;
    double dVal;
    zval *zvalue;

    SaxonProcessor *proc= NULL;
    xdmNode_object *obj = (xdmNode_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
}

PHP_METHOD(XdmNode, __destruct)
{
    xdmNode_object *obj = (xdmNode_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    XdmNode * xdmNode= obj->xdmNode;
   // if(xdmNode != NULL) {
    	xdmNode->decrementRefCount();
    	if(xdmNode->getRefCount()< 1){
    		delete xdmNode;
    	}
    //}
    
}

PHP_METHOD(XdmNode, getStringValue)
{
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;

    if (xdmNode != NULL) {
        const char * valueStr = xdmNode->getStringValue(saxonProc);
        if(valueStr != NULL) {
            char *str = estrdup(valueStr);
            RETURN_STRING(str, 0);
	    return;
        }
    } 
    RETURN_NULL(); 
    
}

PHP_METHOD(XdmNode, getNodeName)
{
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    if (xdmNode != NULL) {
        const char * valueStr = xdmNode->getNodeName();
        if(valueStr != NULL) {
            char *str = estrdup(valueStr);
            RETURN_STRING(str, 0);
        }
    } 
    RETURN_NULL(); 
}

PHP_METHOD(XdmNode, getNodeKind)
{
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    int nodeKind = 0;
    if (xdmNode != NULL) {
        nodeKind = xdmNode->getNodeKind();
        
    }
     RETURN_LONG(nodeKind);
}

PHP_METHOD(XdmNode, isAtomic)
{

    RETURN_FALSE;
}


PHP_METHOD(XdmNode,  getChildCount){
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    int nodeChildCount = 0;
    if (xdmNode != NULL) {
        nodeChildCount = xdmNode->getChildCount();
        
    }
     RETURN_LONG(nodeChildCount);
}   

PHP_METHOD(XdmNode,  getAttributeCount){
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    int nodeAttrCount = 0;
    if (xdmNode != NULL) {
        nodeAttrCount = xdmNode->getAttributeCount();
        
    }
     RETURN_LONG(nodeAttrCount);

} 

PHP_METHOD(XdmNode,  getChildNode){
    int indexi;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&indexi) == FAILURE) {
        RETURN_NULL();
    }

    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    if (xdmNode != NULL) {
	 int count = xdmNode->getChildCount();
	 if(count==0) {
		RETURN_NULL();
		return;
	  }	
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
		
		if(indexi>=0 && indexi < count) {
			XdmNode ** childNodes = xdmNode->getChildren();
			if(childNodes == NULL) {
				RETURN_NULL();
				return;
			}
			XdmNode * childNode = childNodes[indexi];
			if(childNode != NULL) {
				childNode->incrementRefCount();
                		struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                		assert (vobj != NULL);
                		vobj->xdmNode = childNode;
                		return;
			}
		}
            }
    }
    RETURN_NULL();
}

PHP_METHOD(XdmNode,  getParent){
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    if (xdmNode != NULL) {
	XdmNode * parent = xdmNode->getParent();
	if(parent == NULL) {
			RETURN_NULL();
			return;
	}
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
		parent->incrementRefCount();
               	struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
               	assert (vobj != NULL);
               	vobj->xdmNode = parent;
               	return;
            }
    }
    RETURN_NULL();
}

PHP_METHOD(XdmNode,  getAttributeNode){
    int indexi;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&indexi) == FAILURE) {
        RETURN_NULL();
    }

    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    if (xdmNode != NULL) {
	  int count = xdmNode->getAttributeCount();
	  if(count > 0) {
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
		
		if(indexi < count) {
			XdmNode * attNode = xdmNode->getAttributeNodes()[indexi];
			if(attNode != NULL) {
				attNode->incrementRefCount();
                		struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                		assert (vobj != NULL);
                		vobj->xdmNode = attNode;

                		return;
			}
		}
            }
	}
    }
    RETURN_NULL();

}

PHP_METHOD(XdmNode,  getAttributeValue){
   char * name;
   int len1;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;
    if (xdmNode != NULL && name != NULL) {
	
        const char * valueStr = xdmNode->getAttributeValue(name);
        if(valueStr != NULL) {
            char *str = estrdup(valueStr);
            RETURN_STRING(str, 0);
	    return;
        }
    }
    RETURN_NULL();


}

/*     ============== PHP Interface of   XdmAtomicValue =============== */

void xdmAtomicValue_free_storage(void *object TSRMLS_DC)
{
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)object;
    //obj->xdmItem->releaseXdmValue(NULL);
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value xdmAtomicValue_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)emalloc(sizeof(xdmAtomicValue_object));
    memset(obj, 0, sizeof(xdmAtomicValue_object));
   // type->name ="XdmNode";
	 
    obj->std.ce = type;	

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, NULL, xdmNode_free_storage, NULL TSRMLS_CC);
    retval.handlers = &xdmAtomicValue_object_handlers;

    return retval;
}

PHP_METHOD(XdmAtomicValue, __construct)
{
    XdmAtomicValue *xdmValue = NULL;
    bool bVal;
    char * sVal;
    int len;
    long iVal;
    double dVal;
    zval *zvalue;

    xdmAtomicValue_object *obj = (xdmAtomicValue_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

}

PHP_METHOD(XdmAtomicValue, __destruct)
{
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *) zend_object_store_get_object(getThis() TSRMLS_CC);


    XdmAtomicValue * xdmValue= obj->xdmAtomicValue;
    xdmValue->decrementRefCount();
    if(xdmValue->getRefCount()< 1){
    	delete xdmValue;
    }
    
}

PHP_METHOD(XdmAtomicValue, getBooleanValue)
{
    XdmAtomicValue *xdmAtomicValue;
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmAtomicValue = obj->xdmAtomicValue;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    bool resultb = false;
    if (xdmAtomicValue != NULL) {
         resultb = xdmAtomicValue->getBooleanValue();
        
    }
    RETURN_BOOL(resultb);
}


PHP_METHOD(XdmAtomicValue, getDoubleValue)
{
    XdmAtomicValue *xdmAtomicValue;
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmAtomicValue = obj->xdmAtomicValue;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    double resultb = 0;
    if (xdmAtomicValue != NULL) {
         resultb = xdmAtomicValue->getDoubleValue();
        
    }
    RETURN_DOUBLE(resultb);
}

PHP_METHOD(XdmAtomicValue, getLongValue)
{
    XdmAtomicValue *xdmAtomicValue;
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmAtomicValue = obj->xdmAtomicValue;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    long result = 0;
    if (xdmAtomicValue != NULL) {
         result = xdmAtomicValue->getLongValue();
        
    }
    RETURN_LONG(result);
}

PHP_METHOD(XdmAtomicValue, getStringValue)
{
    XdmAtomicValue *xdmAtomicValue;
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmAtomicValue = obj->xdmAtomicValue;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;

    if (xdmAtomicValue != NULL) {
        const char * valueStr = saxonProc->getStringValue(xdmAtomicValue);
        if(valueStr != NULL) {
            char *str = estrdup(valueStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(XdmAtomicValue, isAtomic)
{

    RETURN_TRUE;
}


// =============================================================

zend_function_entry SaxonProcessor_methods[] = {
    PHP_ME(SaxonProcessor,  __construct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(SaxonProcessor,  __destruct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(SaxonProcessor,  createAtomicValue,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  parseXmlFromString,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  parseXmlFromFile,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  setcwd,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  newXPathProcessor,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  newXsltProcessor,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  newXQueryProcessor,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  newSchemaValidator,     NULL, ZEND_ACC_PUBLIC)
//    PHP_ME(SaxonProcessor,  importDocument,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  setResourcesDirectory,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor, setConfigurationProperty,      NULL, ZEND_ACC_PUBLIC)
//    PHP_ME(SaxonProcessor,  registerPHPFunction,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  version,      NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

zend_function_entry XsltProcessor_methods[] = {
    PHP_ME(XsltProcessor,  __destruct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XsltProcessor,  transformFileToFile, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  transformFileToString, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  transformFileToValue, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  transformToString, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  transformToValue, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  transformToFile, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor, compileFromFile, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor, compileFromValue, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor, compileFromString, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setOutputFile, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setSourceFromFile, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setSourceFromXdmValue, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setParameter, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setProperty, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  clearParameters, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  clearProperties, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  exceptionOccurred, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  exceptionClear, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  getErrorCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  getErrorMessage, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  getExceptionCount, NULL, ZEND_ACC_PUBLIC)
{NULL, NULL, NULL}
};

zend_function_entry XQueryProcessor_methods[] = {
    PHP_ME(XQueryProcessor,  __destruct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
   // PHP_ME(XQueryProcessor,  getErrorCode,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  setQueryContent,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  setContextItem,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  setContextItemFromFile,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  setParameter,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  setProperty,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  clearParameters,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  clearProperties,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, runQueryToValue,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, runQueryToString,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, runQueryToFile,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, setQueryFile,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, setQueryBaseURI,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, declareNamespace,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  exceptionOccurred, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  exceptionClear, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  getErrorCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  getErrorMessage, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  getExceptionCount, NULL, ZEND_ACC_PUBLIC)
{NULL, NULL, NULL}
};

zend_function_entry XPathProcessor_methods[] = {
    PHP_ME(XPathProcessor,  __destruct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XPathProcessor,  setContextItem,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  setContextFile,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  effectiveBooleanValue,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  evaluate,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  evaluateSingle,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  setParameter,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  setProperty,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  clearParameters,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  clearProperties,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  exceptionOccurred, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  exceptionClear, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  getErrorCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  getErrorMessage, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  getExceptionCount, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor, declareNamespace,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor, setBaseURI, NULL, ZEND_ACC_PUBLIC)
{NULL, NULL, NULL}
};

zend_function_entry SchemaValidator_methods[] = {
    PHP_ME(SchemaValidator,  __destruct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(SchemaValidator,  setSourceNode,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  setOutputFile,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  validate,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  validateToNode,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  registerSchemaFromFile,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  registerSchemaFromString,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  getValidationReport,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  setParameter,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  setProperty,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  clearParameters,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  clearProperties,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  exceptionOccurred, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  exceptionClear, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  getErrorCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  getErrorMessage, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  getExceptionCount, NULL, ZEND_ACC_PUBLIC)
{NULL, NULL, NULL}
};

zend_function_entry xdmValue_methods[] = {
    PHP_ME(XdmValue,  __construct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(XdmValue,  __destruct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XdmValue,  getHead,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmValue,  itemAt,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmValue,  size,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmValue, addXdmItem,      NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

zend_function_entry xdmItem_methods[] = {
    PHP_ME(XdmItem,  __construct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(XdmItem,  __destruct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XdmItem,  getStringValue,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmItem,  isAtomic,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmItem,  isNode,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmItem,  getAtomicValue,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmItem,  getNodeValue,      NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

zend_function_entry xdmNode_methods[] = {
    PHP_ME(XdmNode,  __construct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(XdmNode,  __destruct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XdmNode,  getStringValue,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getNodeKind,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getNodeName,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  isAtomic,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getChildCount,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getAttributeCount,      NULL, ZEND_ACC_PUBLIC) 
    PHP_ME(XdmNode,  getChildNode,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getParent,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getAttributeNode,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getAttributeValue,      NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

zend_function_entry xdmAtomicValue_methods[] = {
    PHP_ME(XdmAtomicValue,  __construct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(XdmAtomicValue,  __destruct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XdmAtomicValue,  getStringValue,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  isAtomic,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  getBooleanValue,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  getDoubleValue,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  getLongValue,      NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(saxon)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Saxon\\SaxonProcessor", SaxonProcessor_methods);
    saxonProcessor_ce = zend_register_internal_class(&ce TSRMLS_CC);
    saxonProcessor_ce->create_object = saxonProcessor_create_handler;
    memcpy(&saxonProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    saxonProcessor_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Saxon\\XSLTProcessor", XsltProcessor_methods);
    xsltProcessor_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xsltProcessor_ce->create_object = xsltProcessor_create_handler;
    memcpy(&xsltProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xsltProcessor_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Saxon\\XQueryProcessor", XQueryProcessor_methods);
    xqueryProcessor_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xqueryProcessor_ce->create_object = xqueryProcessor_create_handler;
    memcpy(&xqueryProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xqueryProcessor_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Saxon\\XPathProcessor", XPathProcessor_methods);
    xpathProcessor_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xpathProcessor_ce->create_object = xpathProcessor_create_handler;
    memcpy(&xpathProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xpathProcessor_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Saxon\\SchemaValidator", SchemaValidator_methods);
    schemaValidator_ce = zend_register_internal_class(&ce TSRMLS_CC);
    schemaValidator_ce->create_object = schemaValidator_create_handler;
    memcpy(&schemaValidator_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    schemaValidator_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmValue", xdmValue_methods);
    xdmValue_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xdmValue_ce->create_object = xdmValue_create_handler;
    memcpy(&xdmValue_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xdmValue_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmItem", xdmItem_methods);
    xdmItem_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xdmItem_ce->create_object = xdmItem_create_handler;
    memcpy(&xdmItem_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xdmItem_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmNode", xdmNode_methods);
    xdmNode_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xdmNode_ce->create_object = xdmNode_create_handler;
    memcpy(&xdmNode_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xdmNode_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmAtomicValue", xdmAtomicValue_methods);
    xdmAtomicValue_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xdmAtomicValue_ce->create_object = xdmAtomicValue_create_handler;
    memcpy(&xdmAtomicValue_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xdmAtomicValue_object_handlers.clone_obj = NULL;

    return SUCCESS;
}

PHP_MINFO_FUNCTION(saxon)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "Saxon/C", "enabled");
    php_info_print_table_row(2, "Saxon/C EXT version", "1.0.2");
    php_info_print_table_row(2, "Saxon", "9.6.0.9");
    php_info_print_table_row(2, "Excelsior JET", "11 MP3");
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();
}

PHP_MSHUTDOWN_FUNCTION(saxon) {
    UNREGISTER_INI_ENTRIES();
    //SaxonProcessor::release();

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(saxon) {
    //std::cerr<<"RSHUTDOWN called -start"<<std::endl;
    return SUCCESS;
}

PHP_RINIT_FUNCTION(saxon) {
    //std::cerr<<"RINIT called -start"<<std::endl;
    return SUCCESS;
}

zend_module_entry saxon_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_SAXON_EXTNAME,
    NULL,        /* Functions */
    PHP_MINIT(saxon),        /* MINIT */
    PHP_MSHUTDOWN(saxon),        /* MSHUTDOWN */
    NULL,        /* RINIT */
    NULL,        /* RSHUTDOWN */
    PHP_MINFO(saxon),        /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
    PHP_SAXON_EXTVER,
#endif
    STANDARD_MODULE_PROPERTIES
};

