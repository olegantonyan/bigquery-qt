from google.cloud import bigquery


class Project:
    def __init__(self, bq_project: bigquery.client.Project):
        self._proj = bq_project
        #class Project(object):
        #    """Wrapper for resource describing a BigQuery project.
        #    Args:
        #        project_id (str): Opaque ID of the project
        #        numeric_id (int): Numeric ID of the project
        #        friendly_name (str): Display name of the project
        #    """
        #
        #    def __init__(self, project_id, numeric_id, friendly_name):
        #        self.project_id = project_id
        #        self.numeric_id = numeric_id
        #        self.friendly_name = friendly_name
        #
        #    @classmethod
        #    def from_api_repr(cls, resource):
        #        """Factory: construct an instance from a resource dict."""
        #        return cls(resource["id"], resource["numericId"], resource["friendlyName"])

    def id(self) -> str:
        return self._proj.project_id
