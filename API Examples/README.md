# Chirp Hummingbird: Basic API Usage

To make an API call to the Chirp Hummingbird API for small devices, 

```
curl -X POST -d '{ "body": "Hello World!", "title": "Hello", "mimetype": "text/plain" }' -H "X-chirp-hummingbird-key: YOUR-24-CHAR-HUMMINGBIRD-KEY" http://hummingbird.chirp.io:1254/chirp
```

See `Hummingbird.json.postman_collection` for a sample request that can be imported into [Postman](https://chrome.google.com/webstore/detail/postman/fhbjgbiflinjbdggehcddcbncdddomop?hl=en).
