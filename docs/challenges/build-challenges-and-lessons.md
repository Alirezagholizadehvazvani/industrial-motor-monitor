# Build Challenges and Lessons Learned

This document records the real technical, logistical, and practical challenges encountered while developing the Industrial Motor Monitor prototype. These difficulties are included deliberately to demonstrate genuine engineering work under real constraints.

### 1. Strict Budget Constraints

The project operated under tight budget limitations. This was not caused by the company’s inability to fund the work, but by a deliberate commercial decision. In response to prolonged inflation and market pressure, many companies have reduced non-essential product features.

Management and the CEO made the position clear: an unlimited budget could have been provided to develop a sophisticated high-end monitoring system. However, such a system would have remained a purely hypothetical exercise and would never have been deployed. The explicit requirement was therefore to create a functional solution using only the minimum necessary components — either parts already available within the company or items that could be obtained at very low cost.

As a result, several decommissioned company computers were supplied for spare parts. In practice only the power supplies proved useful. A metal enclosure was also provided; it was completely stripped, cleaned, and prepared for reuse.

**Lesson:** Real commercial constraints force prioritization of essential functionality over ideal technical solutions.

### 2. Component Procurement Difficulties

Acquiring electronic modules and development boards was significantly more difficult than expected. International sanctions combined with distorted local market pricing made many required parts either unavailable or prohibitively expensive.

Typical delivery times ranged from two to three weeks. In several cases the received components were faulty and had to be replaced, further extending the timeline.

**Lesson:** In restricted markets, project schedules must include substantial contingency for both availability and quality of components.

### 3. Adaptation of a Standard PC Power Supply

The power source used was a standard ATX computer power supply. Because these units are designed for desktop PCs rather than continuous embedded operation, they would shut down after a single work session unless specific jumper connections were made. Identifying the correct pins and permanently wiring the required jumpers was necessary before reliable operation could be achieved.

### 4. Development of the Data Acquisition Circuit

Building the sensing (data acquisition) circuit was the first major technical task. Although similar circuits had previously been assembled under laboratory supervision, this was the first time the work was performed completely independently. Progress was initially slower than anticipated.

The first wiring diagram used for current measurement proved incorrect and failed to produce accurate ampere readings. Multiple iterations involving different wiring configurations, component values, and measurement methods were required before a reliable design was identified and successfully implemented.

**Lesson:** Independent practical work reveals gaps that supervised laboratory exercises often conceal. The willingness to discard faulty designs and iterate is essential.

### 5. Power and Control Circuits

Construction of the power and control sections was comparatively straightforward due to prior experience. The main challenge in this phase was systematic cable management and organization inside the enclosure, which was completed in a single focused day.

### 6. Staged Testing and Electromagnetic Interference

Direct testing on the target submersible pumps was considered too risky at an early stage because of the high current levels involved. A staged approach using other industrial machines was therefore adopted.

- An industrial saw was tested first. It was later discovered that the saw draws negligible current when running unloaded; meaningful readings appear only when the machine is under actual cutting load.
- Testing was then moved to a welding machine, which draws substantial current and provided useful data.
- The welding machine, however, generated strong electromagnetic interference. When located near the prototype, this noise completely corrupted the sensor readings. Approximately five days of systematic troubleshooting (including longer sensor leads and physical separation) were required to identify the welding machine as the source.

**Key conclusion:** In a real industrial workshop containing multiple high-power machines, the monitoring unit must be physically separated from strong sources of electromagnetic noise. Otherwise the collected data becomes unreliable.

### 7. Motor Startup Inrush Current

Many industrial machines draw significantly higher current during startup than during normal operation. Without compensation, the protection logic would trip immediately on every start-up.

A programmable startup delay of 5–10 seconds was therefore implemented. During this window the protection functions remain disabled, allowing the current to settle to its normal operating level before monitoring begins.

**Lesson:** Effective protection systems must account for normal transient conditions; otherwise they become a source of nuisance trips rather than a reliability improvement.

### 8. Additional Challenges (Restricted)

Further challenges were encountered during testing on actual water pumps at Jahan Pump. Due to company confidentiality restrictions, these details cannot be disclosed in this public repository.

---

## Summary of Key Lessons

- Commercial constraints are often stricter than technical ones and must be treated as design requirements.
- Component availability and quality can dominate the project timeline in restricted markets.
- Electromagnetic compatibility is a first-order concern in real industrial environments.
- Protection logic must be designed around normal machine behavior (inrush, unloaded operation, etc.), not only fault conditions.
- Independent practical experience exposes limitations that laboratory work alone does not reveal.
