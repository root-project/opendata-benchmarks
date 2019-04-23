# Sofware benchmarks with open data

Just an example what is possible with the existing open data.

List of tasks:

1. Plot the Missing ET in an event (loop over events)
2. Plot the Jet pT of all jets in an event (loop over an array in an event)
3. Plot the Jet pT with eta cuts on the jet (loop over an array that is filtered)
4. Plot the missing ET of events with at least 2 jets above 40 GeV (loop over array in event, aggregate those results to filter at event level).
5. Plot missing et for events that have an opposite-sign muon pair of mass 60-120 GeV [looping on combinations of objects in one collection, 4-vector algebra]
6. Plot the pt of the tri-jet system with mass closest to 172.5 GeV, and the leading b-tag discriminator among the 3 jets in the triplet [requires looping on combination of objects in the same collection, and extracting properties of a combination other than the key used to sort them]
7. Plot sum of the pt of all jets of pt > 30 GeV that are not within DR 0.4 from a lepton of pt > 10 GeV [requires looping on two separate collections]
8. In events with >=3 leptons and a same-flavour opposite-sign lepton pair, find the best same-flavour opposite-sign lepton pair (mass closest to 91.2 GeV), and plot the transverse mass of the missing energy and the leading other lepton [ a task whose formulation in an imperative language is easy, but whose translations to a functional query language may be less clear and/or possibly inefficient]
