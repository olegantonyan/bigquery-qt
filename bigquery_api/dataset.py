from google.cloud import bigquery


class Dataset:
    def __init__(self, dataset_ref: bigquery.dataset.DatasetReference):
        self._dataset_ref = dataset_ref

    def id(self) -> str:
        return self._dataset_ref.dataset_id
