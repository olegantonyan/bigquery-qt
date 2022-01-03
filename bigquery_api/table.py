from google.cloud import bigquery


class Table:
    def __init__(self, table_ref: bigquery.table.TableReference):
        self._table_ref = table_ref

    def id(self) -> str:
        return self._table_ref.table_id
