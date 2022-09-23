# MongoDB Study

## Run mongoDB in local

```bash
# Run mongodb
> mongod --dbpath ~/workspace/practice/mongodb/data

# Access mongodb (No need to make admin id/pwd)
> mongosh

# import restaurants collection (in m201 DB)
> mongoimport --drop -c restaurants --uri mongodb://localhost:27017/m201 ~/Downloads/restaurants.json

# show version (in mongo shell)
> db.version()
```

## Summary

- m001 (MongoDB Basic) : <https://medium.com/@aspynlim/m001-mongodb-basics-summary-c8bed947aca2>
- m201 (The MongoDB Performance) : <https://velog.io/@formin/M201-MongoDB-Performance>
- m121 (The MongoDB Aggregation Framework): <https://velog.io/@formin/M121-The-MongoDB-Aggregation-Framework>
- m103 (Basic Cluster Administration) : <https://github.com/thiagoblima/M103-Basic-Cluster-Administration>
