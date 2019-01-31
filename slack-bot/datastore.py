from google.cloud import datastore

class State (object):
    def __init__(self):
       self.ds  = datastore.Client(project='aviv-playground')
       self.kind = 'mastool'

    def put(self,state):
        key = self.ds.key(self.kind, 'state')
        entity = datastore.Entity(key=key)
        entity['state'] = state
        entity.update()
        self.ds.put(entity)

    def get(self):
        key = self.ds.key(self.kind, 'state')
        res = self.ds.get(key)
        return res['state']
