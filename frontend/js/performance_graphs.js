// performance_graphs.js
// Uses D3.js to plot benchmark results showing O(log N) vs O(N) scaling

const BENCHMARK_DATA = [
    { size: 1000, scan: 0.0001, segtree: 0.00008, fenwick: 0.00005, bplus: 0.001 },
    { size: 10000, scan: 0.0010, segtree: 0.00010, fenwick: 0.00006, bplus: 0.010 },
    { size: 100000, scan: 0.0120, segtree: 0.00015, fenwick: 0.00008, bplus: 0.100 },
    { size: 1000000, scan: 0.1250, segtree: 0.00020, fenwick: 0.00010, bplus: 1.250 }
];

const MEMORY_DATA = [
    { name: "Segment Tree", value: 32 },
    { name: "Lazy Segment Tree", value: 54 },
    { name: "Fenwick Tree", value: 16 },
    { name: "B+ Tree", value: 88 }
];

function drawLatencyChart(containerId) {
    const container = d3.select(containerId);
    if (!container.node()) return;
    
    container.selectAll("*").remove();
    
    const margin = { top: 20, right: 100, bottom: 40, left: 60 };
    const width = container.node().getBoundingClientRect().width - margin.left - margin.right;
    const height = 400 - margin.top - margin.bottom;

    const svg = container.append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
        .append("g")
        .attr("transform", `translate(${margin.left},${margin.top})`);

    // Log scales for proper O(N) vs O(log N) visualization
    const x = d3.scaleLog()
        .domain([1000, 1000000])
        .range([0, width]);

    const y = d3.scaleLog()
        .domain([0.00001, 10])
        .range([height, 0]);

    // Axes
    svg.append("g")
        .attr("transform", `translate(0,${height})`)
        .call(d3.axisBottom(x).ticks(4).tickFormat(d => {
            if (d >= 1000000) return "1M";
            if (d >= 1000) return (d / 1000) + "k";
            return d;
        }));

    svg.append("g")
        .call(d3.axisLeft(y).ticks(5).tickFormat(d => d + " ms"));

    // Line generator
    const line = d3.line()
        .x(d => x(d.size))
        .y(d => y(d.val));

    // Data series
    const series = [
        { key: "scan", name: "Seq. Scan O(N)", color: "#ef4444" },
        { key: "segtree", name: "Segment Tree O(log N)", color: "#10b981" },
        { key: "fenwick", name: "Fenwick Tree O(log N)", color: "#f59e0b" },
        { key: "bplus", name: "B+ Tree", color: "#8b5cf6" }
    ];

    series.forEach(s => {
        const plotData = BENCHMARK_DATA.map(d => ({ size: d.size, val: Math.max(0.00001, d[s.key]) }));
        
        svg.append("path")
            .datum(plotData)
            .attr("fill", "none")
            .attr("stroke", s.color)
            .attr("stroke-width", 3)
            .attr("d", line);
            
        // Dots
        svg.selectAll(".dot-" + s.key)
            .data(plotData)
            .enter().append("circle")
            .attr("cx", d => x(d.size))
            .attr("cy", d => y(d.val))
            .attr("r", 4)
            .attr("fill", s.color)
            .attr("stroke", "#0f172a")
            .attr("stroke-width", 2);
            
        // Legend
        svg.append("text")
            .attr("x", width + 10)
            .attr("y", series.indexOf(s) * 20 + 20)
            .style("fill", s.color)
            .style("font-size", "12px")
            .style("font-weight", "500")
            .text(s.name);
    });
}

function drawMemoryChart(containerId) {
    const container = d3.select(containerId);
    if (!container.node()) return;
    
    container.selectAll("*").remove();

    const margin = { top: 30, right: 30, bottom: 60, left: 60 };
    const width = container.node().getBoundingClientRect().width - margin.left - margin.right;
    const height = 400 - margin.top - margin.bottom;

    const svg = container.append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
        .append("g")
        .attr("transform", `translate(${margin.left},${margin.top})`);

    const x = d3.scaleBand()
        .domain(MEMORY_DATA.map(d => d.name))
        .range([0, width])
        .padding(0.3);

    const y = d3.scaleLinear()
        .domain([0, 100])
        .range([height, 0]);

    svg.append("g")
        .attr("transform", `translate(0,${height})`)
        .call(d3.axisBottom(x))
        .selectAll("text")
        .attr("transform", "translate(-10,10)rotate(-45)")
        .style("text-anchor", "end")
        .style("fill", "#94a3b8")
        .style("font-size", "11px");

    svg.append("g")
        .call(d3.axisLeft(y).ticks(5).tickFormat(d => d + " MB"))
        .selectAll("text")
        .style("fill", "#94a3b8");

    // Colors mapping
    const colorMap = {
        "Segment Tree": "#10b981",
        "Lazy Segment Tree": "#3b82f6",
        "Fenwick Tree": "#f59e0b",
        "B+ Tree": "#8b5cf6"
    };

    svg.selectAll("myBar")
        .data(MEMORY_DATA)
        .enter()
        .append("rect")
        .attr("x", d => x(d.name))
        .attr("y", d => y(d.value))
        .attr("width", x.bandwidth())
        .attr("height", d => height - y(d.value))
        .attr("fill", d => colorMap[d.name])
        .attr("rx", 4)
        
    // Value labels
    svg.selectAll("myLabels")
        .data(MEMORY_DATA)
        .enter()
        .append("text")
        .attr("x", d => x(d.name) + x.bandwidth() / 2)
        .attr("y", d => y(d.value) - 10)
        .attr("text-anchor", "middle")
        .style("fill", "white")
        .style("font-size", "12px")
        .style("font-weight", "bold")
        .text(d => d.value + " MB");
}
