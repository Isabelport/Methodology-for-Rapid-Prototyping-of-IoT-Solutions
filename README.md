# Methodology for Rapid Prototyping of IoT Solutions

The Lean Diagnosis is a comprehensive process analysis aimed at identifying potential inefficiencies and facilitating the implementation of optimization strategies. 
The primary objective of this thesis is to create a suite of IoT tools that are non-intrusive and easy to install, specifically designed for gathering data in industrial environments to support Lean Diagnosis. 
To validate the effectiveness of these tools, we have chosen a particular factory as a case study, but these can apply to other areas with similar processes. 
The IoT system comprises three essential components: one for tracking product assembly on the production line, another for detecting employee proximity to locations, and a third for measuring performance of workers engaged in short, repetitive tasks. 
Each of these components comes with a straightforward implementation methodology. 
The findings from these tools align with the factory's overarching goal of improving production efficiency. 
The first node measures dwell times and production times, revealing a correlation between shorter average production times and higher product output when the sample is representative.
The proximity system innovates in the weighting of baseline values and maximum RSSI (Received Signal Strength Indicator). 
Employee allocation varies, with dynamic employees consistently linked to high-output production lines.
Tasks are often resumed on the following day and comparing durations with the reference unveils misclassification and non-standard work patterns. 
Moreover, employee presence state adds reliability and the possibility to classify scheduled and unscheduled breaks. 
System accuracy is evaluated using the testing system or node usage.

The architecture of each node comprises three layers: the sensing, network, and application layers. 
While the sensing layer varies for each node, the remaining layers remain consistent.

<img width="450" alt="all architecture (1)" src="https://github.com/Isabelport/Codigo/assets/48717303/851d8279-c459-4ff0-a7d3-f7079ef8217d">



## Product Tracking Node
This node aims to collect the identification of products along various steps of the production line.

<img width="418" alt="arch rfid (1)" src="https://github.com/Isabelport/Codigo/assets/48717303/5d29b9dd-2048-4f88-ad96-1c8a91beae70">


## Employee's Proximity Node
This node intends to capture the information of the beacon's signal strength to a certain workstation, correlating if the employee carrying that beacon is close, using the technology of BLE. 

<img width="296" alt="architecture_beacon" src="https://github.com/Isabelport/Codigo/assets/48717303/61f52fbc-0624-4e96-9470-73d39bb3fa35">


Read methodology.pdf for instructions on how to implement the nodes.

## Task Monitoring Node

This node collects data from repetitive tasks and presents real-time feedback to the employee using it. 
It identifies the employee, the task, and measures the duration of that task.

<img width="432" alt="architecture_workst" src="https://github.com/Isabelport/Codigo/assets/48717303/9e4a0c30-3177-494f-9b48-9b2325fc74f6">
