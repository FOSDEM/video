- [ ] Make it pretty
- [ ] Make sliders usable with high frontend-backend latency
    - Moving the slider should do calls to the backend in a throttled way
    (don't send a new value if we haven't yet received a response from the previous value)
    - The slider should not jump around while receiving new values while being
    dragged by the user
- [ ] Make it usable from other UIs
    - Other UIs should be able to embed this one and pass their own
    `MisirkaClient` object
