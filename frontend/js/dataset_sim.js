// dataset_sim.js
// Creates a mock time-series dataset and visualizes it using D3.js

class DatasetViz {
    constructor(containerId) {
        this.container = d3.select(containerId);
        this.data = this.generateData(100);
        this.draw();
    }

    generateData(points) {
        let current = 5000;
        const data = [];
        for (let i = 0; i < points; i++) {
            // Random walk with mean reversion
            const noise = (Math.random() - 0.5) * 800;
            current = (current * 0.9 + 5000 * 0.1) + noise;
            data.push({ day: i, sales: Math.max(1000, Math.round(current)) });
        }
        return data;
    }

    draw() {
        const node = this.container.node();
        if (!node) return;
        
        const rect = node.getBoundingClientRect();
        const margin = { top: 20, right: 20, bottom: 30, left: 50 };
        const width = rect.width - margin.left - margin.right;
        const height = rect.height - margin.top - margin.bottom;

        this.container.selectAll("*").remove();

        const svg = this.container.append("svg")
            .attr("width", width + margin.left + margin.right)
            .attr("height", height + margin.top + margin.bottom)
            .append("g")
            .attr("transform", `translate(${margin.left},${margin.top})`);

        // Scales
        const x = d3.scaleLinear()
            .domain(d3.extent(this.data, d => d.day))
            .range([0, width]);

        const y = d3.scaleLinear()
            .domain([0, d3.max(this.data, d => d.sales)])
            .range([height, 0]);

        // Gradient context
        const defs = svg.append("defs");
        const gradient = defs.append("linearGradient")
            .attr("id", "area-gradient")
            .attr("x1", "0%").attr("y1", "0%")
            .attr("x2", "0%").attr("y2", "100%");
        gradient.append("stop")
            .attr("offset", "0%")
            .attr("stop-color", "#3b82f6")
            .attr("stop-opacity", 0.6);
        gradient.append("stop")
            .attr("offset", "100%")
            .attr("stop-color", "#3b82f6")
            .attr("stop-opacity", 0);

        // Area & Line
        const area = d3.area()
            .x(d => x(d.day))
            .y0(height)
            .y1(d => y(d.sales))
            .curve(d3.curveMonotoneX);

        const line = d3.line()
            .x(d => x(d.day))
            .y(d => y(d.sales))
            .curve(d3.curveMonotoneX);

        svg.append("path")
            .datum(this.data)
            .attr("fill", "url(#area-gradient)")
            .attr("d", area);

        svg.append("path")
            .datum(this.data)
            .attr("fill", "none")
            .attr("stroke", "#3b82f6")
            .attr("stroke-width", 2)
            .attr("d", line);

        // Axes
        svg.append("g")
            .attr("transform", `translate(0,${height})`)
            .call(d3.axisBottom(x).ticks(10))
            .selectAll("text")
            .style("fill", "#94a3b8");

        svg.append("g")
            .call(d3.axisLeft(y).ticks(5))
            .selectAll("text")
            .style("fill", "#94a3b8");
            
        // Overlay for selection interaction
        this.brushGroup = svg.append("g").attr("class", "brush");
        this.xScale = x;
    }

    highlightRange(low, high) {
        // Remove previous highlight
        this.container.selectAll(".highlight-rect").remove();
        
        if (low === null || high === null) return;
        
        const svg = this.container.select("g");
        const h = this.container.node().getBoundingClientRect().height - 50; // Approximating internal height
        
        const xLow = Math.max(0, this.xScale(low));
        const xHigh = Math.min(this.xScale.range()[1], this.xScale(high));
        
        if (xLow < xHigh) {
            svg.insert("rect", ":first-child")
                .attr("class", "highlight-rect")
                .attr("x", xLow)
                .attr("y", 0)
                .attr("width", xHigh - xLow)
                .attr("height", h)
                .attr("fill", "rgba(59, 130, 246, 0.2)");
        }
    }
}
