# Title

These short design documents should be "live" documents, being continuously updated as the requirements/design change and development continues.

## Motivation

Why are we writing this component/feature, what is it supposed to resolve? What is the main function/goal? A paragraph should be more than enough to give a high-level description.

### Prototype purpose (if prototype)

What lessons are we trying to learn from this prototype?
It could be that this component is only needed to support other components in the prototype executable. That's fine, but it still has to be specified here.

## Requirements

What are the requirements of this component? Should be listed in bullet-points /
table or some other easy-to-read format. Requirements might be:

- Types of inputs, e.g. `.hdf5`, or `TriggerRecord` or `double` that corresponds to some metric etc.
- Types of outputs, as above.
- Does this have to handle multiple data types? Is this a base class?
- Latency / data input/output rates.
- Does this component have to be configurable? Monitored?
- Any external services needed (e.g. timing service)?

And any other requirements that you can think of. DQM requirements document can be used here for reference.

## Design

A description of the class/feature design. Is it a function, class? Templated? What are the main functions and private/public data members? Can be in bullet points, diagrams, and/or pseudo-code. Should be detailed enough so any DAQ developer can look at it and prototype an implementation.

## Tests

What tests are going to be written to make sure the component works & help with future development? List the unit tests, integration tests, executables etc. that will be written to test the functionality of the component/function.

## Closing criteria

What are the criteria to consider this work done?
Bullet points, boxes are fine. E.g.

- [ ] All unit tests pass.
- [ ] DAQ integration tests with the new feature pass.
- [ ] Implemented in the DQM executable and demonstrated it works.
- [ ] Documentation updated.
