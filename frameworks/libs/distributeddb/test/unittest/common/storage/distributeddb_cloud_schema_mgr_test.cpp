/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "cloud/schema_mgr.h"

#include <gtest/gtest.h>

#include "db_errno.h"
#include "distributeddb_tools_unit_test.h"
#include "relational_store_manager.h"
#include "relational_schema_object.h"
#include "distributeddb_data_generate_unit_test.h"
#include "relational_sync_able_storage.h"
#include "relational_store_instance.h"
#include "sqlite_relational_store.h"
#include "log_table_manager_factory.h"

using namespace testing::ext;
using namespace DistributedDB;
using namespace DistributedDBUnitTest;
using namespace std;

namespace {
    constexpr auto TABLE_NAME_1 = "tableName1";
    constexpr auto TABLE_NAME_2 = "tableName2";
    constexpr auto TABLE_NAME_3 = "tableName3";
    constexpr auto TABLE_NAME_4 = "tableName4";

    constexpr auto FIELD_NAME_1 = "field_name_1";
    constexpr auto FIELD_NAME_2 = "field_name_2";
    constexpr auto FIELD_NAME_3 = "field_name_3";

std::unique_ptr<SchemaMgr> g_schemaMgr = nullptr;

class DistributedDBCloudSchemaMgrTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DistributedDBCloudSchemaMgrTest::SetUpTestCase(void)
{
}

void DistributedDBCloudSchemaMgrTest::TearDownTestCase(void)
{
}

void DistributedDBCloudSchemaMgrTest::SetUp(void)
{
    g_schemaMgr = std::make_unique<SchemaMgr>();
}

void DistributedDBCloudSchemaMgrTest::TearDown(void)
{
    g_schemaMgr = nullptr;
}

DataBaseSchema g_schema = {
    .tables = {
        {
            .name = TABLE_NAME_1,
            .fields = {
                {
                    .colName = FIELD_NAME_1,
                    .type = TYPE_INDEX<int64_t>,
                    .primary = true,
                    .nullable = true,
                },
                {
                    .colName = FIELD_NAME_2,
                    .type = TYPE_INDEX<int64_t>,
                    .primary = false,
                    .nullable = true,
                }
            }
        },
        {
            .name = TABLE_NAME_2,
            .fields = {
                {
                    .colName = FIELD_NAME_1,
                    .type = TYPE_INDEX<int64_t>,
                    .primary = true,
                    .nullable = true,
                },
                {
                    .colName = FIELD_NAME_2,
                    .type = TYPE_INDEX<int64_t>,
                    .primary = false,
                    .nullable = true,
                }
            }
        }
    }
};

DataBaseSchema g_schema2 = {
    .tables = {
        {
            .name = TABLE_NAME_2,
            .fields = {
                {
                    .colName = FIELD_NAME_1,
                    .type = TYPE_INDEX<int64_t>,
                    .primary = true,
                    .nullable = true,
                },
                {
                    .colName = FIELD_NAME_2,
                    .type = TYPE_INDEX<int64_t>,
                    .primary = false,
                    .nullable = true,
                }
            }
        },
        {
            .name = TABLE_NAME_3,
            .fields = {
                {
                    .colName = FIELD_NAME_1,
                    .type = TYPE_INDEX<int64_t>,
                    .primary = true,
                    .nullable = true,
                },
                {
                    .colName = FIELD_NAME_2,
                    .type = TYPE_INDEX<int64_t>,
                    .primary = false,
                    .nullable = true,
                }
            }
        },
        {
            .name = TABLE_NAME_4,
            .fields = {
                {
                    .colName = FIELD_NAME_1,
                    .type = TYPE_INDEX<int64_t>,
                    .primary = true,
                    .nullable = true,
                },
                {
                    .colName = FIELD_NAME_2,
                    .type = TYPE_INDEX<int64_t>,
                    .primary = false,
                    .nullable = true,
                }
            }
        }
    }
};


FieldInfo SetField(std::string fieldName, std::string dataType, bool nullable)
{
    FieldInfo field;
    field.SetFieldName(fieldName);
    field.SetDataType(dataType);
    field.SetNotNull(!nullable);
    return field;
}

/**
  * @tc.name: SchemaMgrTest001
  * @tc.desc: Cloud schema and local schema are not been set
  * test situation: TABLE_NAME exist and not exist(TABLE_NAME_3)
  * @tc.type: FUNC
  * @tc.require:
  * @tc.author: wanyi
  */
HWTEST_F(DistributedDBCloudSchemaMgrTest, SchemaMgrTest001, TestSize.Level0)
{
    FieldInfo field1 = SetField(FIELD_NAME_1, "int", true);
    FieldInfo field2 = SetField(FIELD_NAME_2, "int", true);
    TableInfo table;
    table.SetTableName(TABLE_NAME_2);
    table.AddField(field1);
    table.AddField(field2);
    table.SetPrimaryKey(FIELD_NAME_1, 1);
    RelationalSchemaObject localSchema;
    localSchema.AddRelationalTable(table);

    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_1, localSchema), -E_SCHEMA_MISMATCH);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_2, localSchema), -E_SCHEMA_MISMATCH);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_3, localSchema), -E_SCHEMA_MISMATCH);
}

/**
  * @tc.name: SchemaMgrTest002
  * @tc.desc: Cloud schema and local schema are the same
  * @tc.type: FUNC
  * @tc.require:
  * @tc.author: wanyi
  */
HWTEST_F(DistributedDBCloudSchemaMgrTest, SchemaMgrTest002, TestSize.Level0)
{
    FieldInfo field1 = SetField(FIELD_NAME_1, "int", true);
    FieldInfo field2 = SetField(FIELD_NAME_2, "int", true);
    TableInfo table;
    table.SetTableName(TABLE_NAME_2);
    table.AddField(field1);
    table.AddField(field2);
    table.SetPrimaryKey(FIELD_NAME_1, 1);
    RelationalSchemaObject localSchema;
    localSchema.AddRelationalTable(table);

    g_schemaMgr->SetCloudDbSchema(g_schema);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_2, localSchema), E_OK);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_2, localSchema), E_OK);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_2, localSchema), E_OK);
}

/**
  * @tc.name: SchemaMgrTest003
  * @tc.desc: Local schema contain extra primary key
  * @tc.type: FUNC
  * @tc.require:
  * @tc.author: wanyi
  */
HWTEST_F(DistributedDBCloudSchemaMgrTest, SchemaMgrTest003, TestSize.Level0)
{
    FieldInfo field1 = SetField(FIELD_NAME_1, "int", true);
    FieldInfo field2 = SetField(FIELD_NAME_2, "int", true);
    FieldInfo field3 = SetField(FIELD_NAME_3, "int", true);

    TableInfo table;
    table.SetTableName(TABLE_NAME_2);
    table.AddField(field1);
    table.AddField(field2);
    table.AddField(field3);
    table.SetPrimaryKey(FIELD_NAME_1, 1);
    table.SetPrimaryKey(FIELD_NAME_3, 2);
    RelationalSchemaObject localSchema;
    localSchema.AddRelationalTable(table);

    g_schemaMgr->SetCloudDbSchema(g_schema);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_2, localSchema), -E_SCHEMA_MISMATCH);
}

/**
  * @tc.name: SchemaMgrTest003
  * @tc.desc: Column from local schema is not within cloud schema but is not nullable
  * @tc.type: FUNC
  * @tc.require:
  * @tc.author: wanyi
  */
HWTEST_F(DistributedDBCloudSchemaMgrTest, SchemaMgrTest004, TestSize.Level0)
{
    FieldInfo field1 = SetField(FIELD_NAME_1, "int", true);
    FieldInfo field2 = SetField(FIELD_NAME_2, "int", true);
    FieldInfo field3 = SetField(FIELD_NAME_2, "int", false);

    TableInfo table;
    table.SetTableName(TABLE_NAME_2);
    table.AddField(field1);
    table.AddField(field2);
    table.AddField(field3);
    table.SetPrimaryKey(FIELD_NAME_1, 1);
    RelationalSchemaObject localSchema;
    localSchema.AddRelationalTable(table);

    g_schemaMgr->SetCloudDbSchema(g_schema);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_2, localSchema), -E_SCHEMA_MISMATCH);
}

/**
  * @tc.name: SchemaMgrTest003
  * @tc.desc: Local schema contain extra noraml key with default value but cannot be null
  * @tc.type: FUNC
  * @tc.require:
  * @tc.author: wanyi
  */
HWTEST_F(DistributedDBCloudSchemaMgrTest, SchemaMgrTest005, TestSize.Level0)
{
    FieldInfo field1 = SetField(FIELD_NAME_1, "int", true);
    FieldInfo field2 = SetField(FIELD_NAME_2, "int", true);
    FieldInfo field3 = SetField(FIELD_NAME_3, "int", false);
    field3.SetDefaultValue("0");

    TableInfo table;
    table.SetTableName(TABLE_NAME_2);
    table.AddField(field1);
    table.AddField(field2);
    table.AddField(field3);
    table.SetPrimaryKey(FIELD_NAME_1, 1);
    RelationalSchemaObject localSchema;
    localSchema.AddRelationalTable(table);

    g_schemaMgr->SetCloudDbSchema(g_schema);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_2, localSchema), E_OK);
}

/**
  * @tc.name: SchemaMgrTest003
  * @tc.desc: Local schema contain extra noraml key with default value but cannot be null
  * @tc.type: FUNC
  * @tc.require:
  * @tc.author: wanyi
  */
HWTEST_F(DistributedDBCloudSchemaMgrTest, SchemaMgrTest006, TestSize.Level0)
{
    FieldInfo field1 = SetField(FIELD_NAME_1, "int", true);
    FieldInfo field2 = SetField(FIELD_NAME_2, "int", true);
    FieldInfo field3 = SetField(FIELD_NAME_3, "int", true);
    field3.SetDefaultValue("0");

    TableInfo table;
    table.SetTableName(TABLE_NAME_2);
    table.AddField(field1);
    table.AddField(field2);
    table.AddField(field3);
    table.SetPrimaryKey(FIELD_NAME_1, 1);
    RelationalSchemaObject localSchema;
    localSchema.AddRelationalTable(table);

    g_schemaMgr->SetCloudDbSchema(g_schema);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_2, localSchema), E_OK);
}

/**
  * @tc.name: SchemaMgrTest003
  * @tc.desc: Local schema contain extra noraml key with default value but cannot be null
  * @tc.type: FUNC
  * @tc.require:
  * @tc.author: wanyi
  */
HWTEST_F(DistributedDBCloudSchemaMgrTest, SchemaMgrTest007, TestSize.Level0)
{
    FieldInfo field1 = SetField(FIELD_NAME_1, "int", true);
    FieldInfo field2 = SetField(FIELD_NAME_2, "int", true);
    FieldInfo field3 = SetField(FIELD_NAME_3, "int", true);

    TableInfo table;
    table.SetTableName(TABLE_NAME_2);
    table.AddField(field1);
    table.AddField(field2);
    table.AddField(field3);
    table.SetPrimaryKey(FIELD_NAME_1, 1);
    RelationalSchemaObject localSchema;
    localSchema.AddRelationalTable(table);

    g_schemaMgr->SetCloudDbSchema(g_schema);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_2, localSchema), E_OK);
}

/**
  * @tc.name: SchemaMgrTest008
  * @tc.desc: Cloud schema or local schema are not exist
  * @tc.type: FUNC
  * @tc.require:
  * @tc.author: wanyi
  */
HWTEST_F(DistributedDBCloudSchemaMgrTest, SchemaMgrTest008, TestSize.Level0)
{
    FieldInfo field1 = SetField(FIELD_NAME_1, "int", true);
    FieldInfo field2 = SetField(FIELD_NAME_2, "int", true);
    TableInfo table;
    table.SetTableName(TABLE_NAME_3);
    table.AddField(field1);
    table.AddField(field2);
    table.SetPrimaryKey(FIELD_NAME_1, 1);

    TableInfo table2;
    table2.SetTableName(TABLE_NAME_1);
    table2.AddField(field1);
    table2.AddField(field2);
    table2.SetPrimaryKey(FIELD_NAME_1, 1);

    RelationalSchemaObject localSchema;
    localSchema.AddRelationalTable(table);
    localSchema.AddRelationalTable(table2);

    g_schemaMgr->SetCloudDbSchema(g_schema);
    // local schema exist but cloud schema not exist
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_3, localSchema), -E_SCHEMA_MISMATCH);
    // cloud schema exist but local schema not exist
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_2, localSchema), -E_SCHEMA_MISMATCH);
    // Both cloud schema and local schema does not exist
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_4, localSchema), -E_SCHEMA_MISMATCH);
    // Both cloud schema and local schema exist
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_1, localSchema), E_OK);

    g_schemaMgr->SetCloudDbSchema(g_schema2);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_3, localSchema), E_OK);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_1, localSchema), -E_SCHEMA_MISMATCH);
}

/**
  * @tc.name: SchemaMgrTest008
  * @tc.desc: Test schema mgr with empty local schema
  * @tc.type: FUNC
  * @tc.require:
  * @tc.author: wanyi
  */
HWTEST_F(DistributedDBCloudSchemaMgrTest, SchemaMgrTest009, TestSize.Level0)
{
    RelationalSchemaObject localSchema;
    g_schemaMgr->SetCloudDbSchema(g_schema);
    EXPECT_EQ(g_schemaMgr->ChkSchema(TABLE_NAME_1, localSchema), -E_SCHEMA_MISMATCH);
}

}