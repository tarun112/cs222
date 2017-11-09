#include <cstdio>
#include <cstring>
#include <iostream>
#include <cmath>
#include "rm.h"

const string TABLES_FILE_NAME = "Tables";
const string COLUMNS_FILE_NAME = "Columns";

RelationManager* RelationManager::instance()
{
    static RelationManager _rm;
    return &_rm;
}

RelationManager::RelationManager()
{
}

RelationManager::~RelationManager()
{
}

int createTablesAttribs(vector<Attribute>& tblAttrs)
{
    Attribute attr;
    attr.name = "table-id";
    attr.type = TypeInt;
    attr.length = 4;
    tblAttrs.push_back(attr);

    attr.name = "table-name";
    attr.type = TypeVarChar;
    attr.length = 50;
    tblAttrs.push_back(attr);

    attr.name = "file-name";
    attr.type = TypeVarChar;
    attr.length = 50;
    tblAttrs.push_back(attr);

    attr.name = "system-table";
    attr.type = TypeInt;
    attr.length = 4;
    tblAttrs.push_back(attr);
    
    return 0;
}


int createTablesData(const vector<Attribute>& tblAttrs, int tableid, const string &tableName, int issystem, void* data)
{
    char nulldesc;
    memset(&nulldesc, 0, 1);
    int offset = 0;
    memcpy((char*)data, &nulldesc, sizeof(char));
    offset += sizeof(char);

    memcpy((char*)data + offset, (char*)&tableid, sizeof(int));
    offset += sizeof(int);

    int len = tableName.length();
    memcpy((char*)data + offset, (char*)&len, sizeof(int));
    offset += sizeof(int);

    const char* tblname = tableName.c_str();
    memcpy((char*)data + offset, (char*)tblname, len * sizeof(char));
    offset += len * sizeof(char);

    len = tableName.length();
    memcpy((char*)data + offset, (char*)&len, sizeof(int));
    offset += sizeof(int);

    const char* fname = tableName.c_str();
    memcpy((char*)data + offset, (char*)fname, len * sizeof(char));
    offset += len * sizeof(char);

    memcpy((char*)data + offset, (char*)&issystem, sizeof(int));

    return 0;
}

int createColumnsAttribs(vector<Attribute>& clmAttrs)
{
    Attribute attr;
    attr.name = "table-id";
    attr.type = TypeInt;
    attr.length = 4;
    clmAttrs.push_back(attr);

    attr.name = "column-name";
    attr.type = TypeVarChar;
    attr.length = 50;
    clmAttrs.push_back(attr);

    attr.name = "column-type";
    attr.type = TypeInt;
    attr.length = 4;
    clmAttrs.push_back(attr);

    attr.name = "column-length";
    attr.type = TypeInt;
    attr.length = 4;
    clmAttrs.push_back(attr);

    attr.name = "column-position";
    attr.type = TypeInt;
    attr.length = 4;
    clmAttrs.push_back(attr);

    attr.name = "column-exists";
    attr.type = TypeInt;
    attr.length = 4;
    clmAttrs.push_back(attr);

    return 0;
}

int createColumnsAttribData(const vector<Attribute>& clmAttrs, const Attribute& attrib, 
    int tableid, int position, void* data)
{
    char nulldesc;
    memset(&nulldesc, 0, 1);
    int offset = 0;
    memcpy((char*)data, &nulldesc, sizeof(char));
    offset += sizeof(char);

    memcpy((char*)data + offset, (char*)&tableid, sizeof(int));
    offset += sizeof(int);

    int len = attrib.name.length();
    memcpy((char*)data + offset, (char*)&len, sizeof(int));
    offset += sizeof(int);

    const char* clmname = attrib.name.c_str();
    memcpy((char*)data + offset, (char*)clmname, len * sizeof(char));
    offset += len * sizeof(char);

    int clmtype = attrib.type;
    memcpy((char*)data + offset, (char*)&clmtype, sizeof(int));
    offset += sizeof(int);

    int clmlen = attrib.length;
    memcpy((char*)data + offset, (char*)&clmlen, sizeof(int));
    offset += sizeof(int);

    memcpy((char*)data + offset, (char*)&position, sizeof(int));
    offset += sizeof(int);

    int exists = 0;
    memcpy((char*)data + offset, (char*)&exists, sizeof(int));
    return 0;
}

//int createColumnsData(const vector<Attribute>& clmAttrs, int tableid, void* data)
//{
//
//    return 0;
//}
//

//int createColumnsFile()
//{
//    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
//    RC stat = 0;
//
//    string clmFName = "Columns";
//    stat = rbfm->createFile(clmFName);
//    FileHandle clmFHndl;
//    stat = rbfm->openFile(clmFName, clmFHndl);
//
//    vector<Attribute> clmAttrs;
//    createColumnsAttribs(clmAttrs);
//
//    int nattr = clmAttrs.size();
//    void* data = malloc(200);
//    for (int i = 0; i < nattr; ++i) {
//        int pos = i + 1;
//        createColumnsAttribData(clmAttrs[i], tableid, pos, data);
//    }
//
//    void* data = malloc(clmRecSize);
//    RID rid;
//    stat = rbfm->insertRecord(clmFHndl, clmAttrs, data, rid);
//    stat = rbfm->closeFile(clmFHndl);
//
//
//}

//int insertSystemFiles


RC RelationManager::createCatalog()
{
    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
    RC stat = 0;
    stat = rbfm->createFile(TABLES_FILE_NAME);
    FileHandle tblFHndl;
    rbfm->openFile(TABLES_FILE_NAME, tblFHndl);

    stat = rbfm->createFile(COLUMNS_FILE_NAME);
    FileHandle clmFHndl;
    rbfm->openFile(COLUMNS_FILE_NAME, clmFHndl);

    RID rid;

    vector<Attribute> tblAttrbs;
    createTablesAttribs(tblAttrbs);
    int tableid = 1;
    int issytem = 1;

    void* data = malloc(300);
    
    createTablesData(tblAttrbs, tableid, TABLES_FILE_NAME, issytem, data);
    stat = rbfm->insertRecord(tblFHndl, tblAttrbs, data, rid);

//    rbfm->readRecord(tblFHndl, tblAttrbs, rid, data);
//    rbfm->printRecord(tblAttrbs, data);

    vector<Attribute> clmAttrbs;
    createColumnsAttribs(clmAttrbs);

    int nattr = tblAttrbs.size();
    for (int i = 0; i < nattr; ++i) {
        int pos = i + 1;
        createColumnsAttribData(clmAttrbs, tblAttrbs[i], tableid, pos, data);
        stat = rbfm->insertRecord(clmFHndl, clmAttrbs, data, rid);
    }

    tableid = 2;
    issytem = 1;
    createTablesData(tblAttrbs, tableid, COLUMNS_FILE_NAME, issytem, data);
    stat = rbfm->insertRecord(tblFHndl, tblAttrbs, data, rid);

    nattr = clmAttrbs.size();
    for (int i = 0; i < nattr; ++i) {
        int pos = i + 1;
        createColumnsAttribData(clmAttrbs, clmAttrbs[i], tableid, pos, data);
        stat = rbfm->insertRecord(clmFHndl, clmAttrbs, data, rid);
    }

    stat = rbfm->closeFile(tblFHndl);
    stat = rbfm->closeFile(clmFHndl);
    free(data);
    return 0;
}

RC RelationManager::deleteCatalog()
{
    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
    RC stat = 0;
    stat = rbfm->destroyFile(TABLES_FILE_NAME);
    if (stat) return stat;
    rbfm->destroyFile(COLUMNS_FILE_NAME);
    if (stat) return stat;
    return 0;
}

int getMaxDataSize(const vector<Attribute> &attrs) 
{
    int maxsize = 0;
    for (int i = 0; i < attrs.size(); ++i) {
        maxsize += attrs[i].length;
    }
    maxsize += ceil(attrs.size() / 8.0);
    return maxsize;
}

int getNextTableID() {
    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
    RC stat = 0;
    FileHandle tblFHndl;
    stat = rbfm->openFile(TABLES_FILE_NAME, tblFHndl);
    if (stat) return stat;

    vector<Attribute> tblAttrbs;
    createTablesAttribs(tblAttrbs);
    int tableid = -1;
    int issytem = 1;

    int maxtableid = -1;
    RBFM_ScanIterator rbfmit;
    void *value = NULL;
    vector<string> matchattrbs;
    matchattrbs.push_back(string("table-id"));
    rbfm->scan(tblFHndl, tblAttrbs, string(""), NO_OP, value, matchattrbs, rbfmit);

    RID rid;
    void *data = malloc(200);
    while (RBFM_EOF != rbfmit.getNextRecord(rid, data)) {
        memcpy((char*)&tableid, (char*)data+1, sizeof(int));
        if (tableid > maxtableid)
            maxtableid = tableid;
    }
    free(data);

    stat = rbfmit.close();
    if (stat) return stat;

    return maxtableid+1;
}

RC RelationManager::createTable(const string &tableName, const vector<Attribute> &attrs)
{
    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
    RC stat = 0;

    string tblFName = tableName;
    stat = rbfm->createFile(tblFName);


    FileHandle tblFHndl;
    stat = rbfm->openFile(TABLES_FILE_NAME, tblFHndl);
    FileHandle clmFHndl;
    stat = rbfm->openFile(COLUMNS_FILE_NAME, clmFHndl);

    RID rid;

    vector<Attribute> tblAttrs;
    createTablesAttribs(tblAttrs);
    int tableid = getNextTableID();
    int issystem = 0;
    void* data = malloc(300);
    int res = createTablesData(attrs, tableid, tableName, issystem, data);
    rbfm->insertRecord(tblFHndl, tblAttrs, data, rid);

    vector<Attribute> clmAttrs;
    createColumnsAttribs(clmAttrs);
    int nattr = attrs.size();
    for (int i = 0; i < nattr; ++i) {
        int pos = i + 1;
        createColumnsAttribData(clmAttrs, attrs[i], tableid, pos, data);
        stat = rbfm->insertRecord(clmFHndl, clmAttrs, data, rid);
    }

    stat = rbfm->closeFile(tblFHndl);
    stat = rbfm->closeFile(clmFHndl);
    free(data);

    return 0;
}

int getTableId(const string &tableName) 
{
    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
    RC stat = 0;
    FileHandle tblFHndl;
    stat = rbfm->openFile(TABLES_FILE_NAME, tblFHndl);
    if (stat) return stat;

    vector<Attribute> tblAttrbs;
    createTablesAttribs(tblAttrbs);
    int tableid = -1;
    int issytem = 1;

    RBFM_ScanIterator rbfmit;
    void *value = malloc(200);
    int len = tableName.length();
    memcpy((char*)value, (char*)&len, sizeof(int));
    memcpy((char*)value + sizeof(int), tableName.c_str(), len * sizeof(char));
    vector<string> matchattrbs;
    matchattrbs.push_back(string("table-id"));
    stat = rbfm->scan(tblFHndl, tblAttrbs, string("table-name"), EQ_OP, value, matchattrbs, rbfmit);
    if (stat) {
        free(value);
        return stat;
    }

    RID rid;
    void *data = malloc(200);
    if (RBFM_EOF != rbfmit.getNextRecord(rid, data)) {
        memcpy((char*)&tableid, (char*)data + 1, sizeof(int));
    }

    free(value);
    free(data);

    stat = rbfmit.close();
    if (stat) return stat;
    
    stat = rbfm->closeFile(tblFHndl);
    if (stat) return stat;

    return tableid;
}

int getTableIdAndSystem(const string &tableName, int &tableid, int &issystem)
{
    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
    RC stat = 0;
    FileHandle tblFHndl;
    stat = rbfm->openFile(TABLES_FILE_NAME, tblFHndl);
    if (stat) return stat;

    vector<Attribute> tblAttrbs;
    createTablesAttribs(tblAttrbs);

    RBFM_ScanIterator rbfmit;
    void *value = malloc(200);
    int len = tableName.length();
    memcpy((char*)value, (char*)&len, sizeof(int));
    memcpy((char*)value + sizeof(int), tableName.c_str(), len * sizeof(char));
    vector<string> matchattrbs;
    matchattrbs.push_back(string("table-id"));
    matchattrbs.push_back(string("system-table"));

    stat = rbfm->scan(tblFHndl, tblAttrbs, string("table-name"), EQ_OP, value, matchattrbs, rbfmit);
    if (stat) {
        free(value);
        return stat;
    }

    RID rid;
    void *data = malloc(200);
    if (RBFM_EOF != rbfmit.getNextRecord(rid, data)) {
        memcpy((char*)&tableid, (char*)data + 1, sizeof(int));
        memcpy((char*)&issystem, (char*)data + 1 + sizeof(int), sizeof(int));
    }

    free(value);
    free(data);

    stat = rbfmit.close();
    if (stat) return stat;

    stat = rbfm->closeFile(tblFHndl);
    if (stat) return stat;

    return 0;
}


RC RelationManager::deleteTable(const string &tableName)
{
    int tableid = -1; // getTableId(tableName);
    int issystem = -1;
    getTableIdAndSystem(tableName, tableid, issystem);
    if (tableid < 0 || issystem == 1)
        return -1;

    RID rid;

    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
    RC stat = 0;

    FileHandle tblFHndl;
    stat = rbfm->openFile(TABLES_FILE_NAME, tblFHndl);
    FileHandle clmFHndl;
    stat = rbfm->openFile(COLUMNS_FILE_NAME, clmFHndl);

    vector<Attribute> tblAttrs;
    createTablesAttribs(tblAttrs);
    rbfm->deleteRecord(tblFHndl, tblAttrs, rid);

    vector<Attribute> clmAttrs;
    createColumnsAttribs(clmAttrs);

    RBFM_ScanIterator rbfmit;
    vector<string> matchattrbs;
    matchattrbs.push_back(string("column-name"));
    stat = rbfm->scan(clmFHndl, clmAttrs, string("table-id"), EQ_OP, (void*)&tableid, matchattrbs, rbfmit);
    if (stat) return stat;

    void *data = malloc(200);
    vector<RID> clmrids;
    while (RBFM_EOF != rbfmit.getNextRecord(rid, data)) {
        clmrids.push_back(rid);
    }
    free(data);

    stat = rbfmit.close();
    if (stat) return stat;

    int nattr = clmrids.size();
    for (int i = 0; i < nattr; ++i) {
        stat = rbfm->deleteRecord(clmFHndl, clmAttrs, clmrids[i]);
        if (stat) return stat;
    }

    stat = rbfm->closeFile(tblFHndl);
    if (stat) return stat;

    stat = rbfm->closeFile(clmFHndl);
    if (stat) return stat;

    stat = rbfm->destroyFile(tableName);
    if (stat) return stat;

    return 0;
}

RC RelationManager::getAttributes(const string &tableName, vector<Attribute> &attrs)
{
    RID rid;
    int tableid = getTableId(tableName);
    if (tableid < 0 )
        return -1;

    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
    RC stat = 0;

    FileHandle clmFHndl;
    stat = rbfm->openFile(COLUMNS_FILE_NAME, clmFHndl);

    vector<Attribute> clmAttrs;
    createColumnsAttribs(clmAttrs);

    RBFM_ScanIterator rbfmit;
    vector<string> matchattrbs;
    matchattrbs.push_back(string("column-name"));
    matchattrbs.push_back(string("column-type"));
    matchattrbs.push_back(string("column-length"));
    matchattrbs.push_back(string("column-exists"));

    stat = rbfm->scan(clmFHndl, clmAttrs, string("table-id"), EQ_OP, (void*)&tableid, matchattrbs, rbfmit);
    if (stat) return stat;

    void *data = malloc(200);
    while (RBFM_EOF != rbfmit.getNextRecord(rid, data)) {
        int offset = 1;
        Attribute attrb;
        int len;
        memcpy((char*)&len, (char*)data + offset, sizeof(int));
        offset += sizeof(int);
        char name[50];
        memcpy(name, (char*)data + offset, len * sizeof(char));
        offset += len * sizeof(char);
        name[len] = '\0';
        attrb.name = name;
        memcpy((char*)&attrb.type, (char*)data + offset, sizeof(int));
        offset += sizeof(int);
        memcpy((char*)&attrb.length, (char*)data + offset, sizeof(int));
        offset += sizeof(int);
        int exists = 0;
        memcpy((char*)&exists, (char*)data + offset, sizeof(int));
        if (exists == 1)
            continue;
        attrs.push_back(attrb);
    }
    free(data);

    stat = rbfmit.close();
    if (stat) return stat;

    stat = rbfm->closeFile(clmFHndl);
    if (stat) return stat;

    return 0;
}

RC RelationManager::insertTuple(const string &tableName, const void *data, RID &rid)
{
    int tableid = -1; // getTableId(tableName);
    int issystem = -1;
    getTableIdAndSystem(tableName, tableid, issystem);
    if (tableid < 0 || issystem == 1)
        return -1;

    vector<Attribute> attrs;
    RC stat = 0;
    stat = getAttributes(tableName, attrs);
    if (stat) return stat;

    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();

    FileHandle fileHndl;
    stat = rbfm->openFile(tableName, fileHndl);
    if (stat) return stat;

    stat = rbfm->insertRecord(fileHndl, attrs, data, rid);
    if (stat) return stat;

    stat = rbfm->closeFile(fileHndl);
    if (stat) return stat;

    return 0;
}

RC RelationManager::deleteTuple(const string &tableName, const RID &rid)
{
    int tableid = -1; // getTableId(tableName);
    int issystem = -1;
    getTableIdAndSystem(tableName, tableid, issystem);
    if (tableid < 0 || issystem == 1)
        return -1;

    vector<Attribute> attrs;
    RC stat = 0;
    stat = getAttributes(tableName, attrs);
    if (stat) return stat;

    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();

    FileHandle fileHndl;
    stat = rbfm->openFile(tableName, fileHndl);
    if (stat) return stat;

    stat = rbfm->deleteRecord(fileHndl, attrs, rid);
    if (stat) return stat;

    stat = rbfm->closeFile(fileHndl);
    if (stat) return stat;

    return 0;
}

RC RelationManager::updateTuple(const string &tableName, const void *data, const RID &rid)
{
    int tableid = -1; // getTableId(tableName);
    int issystem = -1;
    getTableIdAndSystem(tableName, tableid, issystem);
    if (tableid < 0 || issystem == 1)
        return -1;

    vector<Attribute> attrs;
    RC stat = 0;
    stat = getAttributes(tableName, attrs);
    if (stat) return stat;

    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();

    FileHandle fileHndl;
    stat = rbfm->openFile(tableName, fileHndl);
    if (stat) return stat;

    stat = rbfm->updateRecord(fileHndl, attrs, data, rid);
    if (stat) return stat;

    stat = rbfm->closeFile(fileHndl);
    if (stat) return stat;

    return 0;
}

RC RelationManager::readTuple(const string &tableName, const RID &rid, void *data)
{
    vector<Attribute> attrs;
    RC stat = 0;
    stat = getAttributes(tableName, attrs);
    if (stat) return stat;

    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();

    FileHandle fileHndl;
    stat = rbfm->openFile(tableName, fileHndl);
    if (stat) return stat;

    stat = rbfm->readRecord(fileHndl, attrs, rid, data);
    if (stat) {
        rbfm->closeFile(fileHndl);
        return stat;
    }

    stat = rbfm->closeFile(fileHndl);
    if (stat) return stat;

    return 0;
}

RC RelationManager::printTuple(const vector<Attribute> &attrs, const void *data)
{
    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
    RC stat = 0;

    stat = rbfm->printRecord(attrs, data);
    if (stat) return stat;

    return 0;
}

bool isAttributeDropped(vector<Attribute>& attrs, const string& attributeName)
{
    int sz = attrs.size();
    for (const Attribute& attrb : attrs)
        if (attrb.name == attributeName)
            return true;
    return false;
}

RC RelationManager::readAttribute(const string &tableName, const RID &rid, const string &attributeName, void *data)
{
    vector<Attribute> attrs;
    RC stat = 0;
    stat = getAttributes(tableName, attrs);
    if (stat) return stat;

    if (isAttributeDropped(attrs, attributeName)) {
        return -1;
    }

    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();

    FileHandle fileHndl;
    stat = rbfm->openFile(tableName, fileHndl);
    if (stat) return stat;

    stat = rbfm->readAttribute(fileHndl, attrs, rid, attributeName, data);
    if (stat) return stat;

    stat = rbfm->closeFile(fileHndl);
    if (stat) return stat;

    return 0;
}

RC RelationManager::scan(const string &tableName,
      const string &conditionAttribute,
      const CompOp compOp,                  
      const void *value,                    
      const vector<string> &attributeNames,
      RM_ScanIterator &rm_ScanIterator)
{
    vector<Attribute> attrs;
    RC stat = 0;
    stat = getAttributes(tableName, attrs);
    if (stat) return stat;

    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();

    stat = rbfm->openFile(tableName, rm_ScanIterator.fileHndl);
    if (stat) return stat;

    stat = rbfm->scan(rm_ScanIterator.fileHndl, attrs, conditionAttribute, compOp, value, attributeNames, rm_ScanIterator.rbfmit);
    if (stat) return stat;

    return 0;
}

RC RM_ScanIterator::getNextTuple(RID &rid, void *data) 
{ 
    return rbfmit.getNextRecord(rid, data); 
}

RC RM_ScanIterator::close() 
{ 
    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
    RC stat = 0;
    stat = rbfmit.close();
    if (stat) return stat;

    stat = rbfm->closeFile(fileHndl);
    if (stat) return stat;

    return 0;
}

// Extra credit work
RC RelationManager::dropAttribute(const string &tableName, const string &attributeName)
{
    int tableid = -1; // getTableId(tableName);
    int issystem = -1;
    RID rid;
    getTableIdAndSystem(tableName, tableid, issystem);
    if (tableid < 0 || issystem == 1)
        return -1;

    RecordBasedFileManager* rbfm = RecordBasedFileManager::instance();
    RC stat = 0;

    FileHandle clmFHndl;
    stat = rbfm->openFile(COLUMNS_FILE_NAME, clmFHndl);

    vector<Attribute> clmAttrs;
    createColumnsAttribs(clmAttrs);

    RBFM_ScanIterator rbfmit;
    vector<string> matchattrbs;
    matchattrbs.push_back(string("column-name"));
    stat = rbfm->scan(clmFHndl, clmAttrs, string("table-id"), EQ_OP, (void*)&tableid, matchattrbs, rbfmit);
    if (stat) return stat;

    void *data = malloc(200);
    RID rmRid;
    bool found = false;
    while (RBFM_EOF != rbfmit.getNextRecord(rid, data)) {
        int len = 0;
        memcpy((char*)&len, (char*)data + 1, sizeof(int));
        char attribname[51];
        memcpy(attribname, (char*)data + 1 + sizeof(int), len * sizeof(char));
        attribname[len] = '\0';
        if (! attributeName.compare(attribname)) {
            rmRid = rid;
            found = true;
            break;
        }
    }
    free(data);

    stat = rbfmit.close();
    if (stat) return stat;

    if (found) {
        void *data = malloc(200);
        stat = rbfm->readRecord(clmFHndl, clmAttrs, rmRid, data);
        if (stat) {
            stat = rbfm->closeFile(clmFHndl);
            return stat;
        }
        int datasz = rbfm->getDataSize(clmAttrs, data);
        int exists = 1;
        memcpy((char*)data+(datasz-sizeof(int)), (char*)&exists, sizeof(int));
        stat = rbfm->updateRecord(clmFHndl, clmAttrs, data, rmRid);
        if (stat) {
            stat = rbfm->closeFile(clmFHndl);
            return stat;
        }
        free(data);
    }

    stat = rbfm->closeFile(clmFHndl);
    if (stat) return stat;

    return 0;
}

// Extra credit work
RC RelationManager::addAttribute(const string &tableName, const Attribute &attr)
{
    return -1;
}


