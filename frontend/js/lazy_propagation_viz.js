// lazy_propagation_viz.js
// Visualizes lazy propagation updates pushing down the tree

class LazyPropagationViz {
    constructor(containerId) {
        this.container = d3.select(containerId);
        this.initSVG();
    }

    initSVG() {
        if (!this.container.node()) return;
        
        const width = this.container.node().getBoundingClientRect().width;
        const height = 200;

        this.container.selectAll("*").remove();

        this.svg = this.container.append("svg")
            .attr("width", width)
            .attr("height", height)
            .append("g")
            .attr("transform", "translate(0,20)");
            
        // Initial setup for lazy prop demo (small subtree)
        this.nodes = [
            { id: 1, text: "Sum: 200", lazy: "+10", level: 0 },
            { id: 2, text: "Sum: 120", lazy: " 0 ", level: 1, side: "left" },
            { id: 3, text: "Sum:  80", lazy: " 0 ", level: 1, side: "right" }
        ];

        this.draw();
    }

    draw() {
        const width = this.svg.node().parentNode.getBoundingClientRect().width;
        const cx = width / 2;

        const nodeGroup = this.svg.selectAll(".lazy-node")
            .data(this.nodes)
            .enter().append("g")
            .attr("class", d => `lazy-node lazy-node-${d.id}`)
            .attr("transform", d => {
                if (d.level === 0) return `translate(${cx}, 20)`;
                return `translate(${d.side === 'left' ? cx - 80 : cx + 80}, 100)`;
            });

        // Main node box
        nodeGroup.append("rect")
            .attr("width", 70)
            .attr("height", 40)
            .attr("x", -35)
            .attr("y", -20)
            .attr("rx", 6)
            .attr("fill", "var(--card-bg)")
            .attr("stroke", "var(--border-color)");

        nodeGroup.append("text")
            .attr("dy", 4)
            .attr("text-anchor", "middle")
            .style("fill", "white")
            .style("font-family", "monospace")
            .text(d => d.text);

        // Lazy tag
        const lazyTag = nodeGroup.append("g")
            .attr("class", "lazy-tag")
            .attr("transform", "translate(0, -30)");

        lazyTag.append("rect")
            .attr("width", 40)
            .attr("height", 20)
            .attr("x", -20)
            .attr("y", -10)
            .attr("rx", 4)
            .attr("fill", d => d.lazy !== " 0 " ? "var(--node-lazy)" : "var(--node-default)");

        lazyTag.append("text")
            .attr("dy", 4)
            .attr("text-anchor", "middle")
            .style("fill", "white")
            .style("font-size", "10px")
            .text(d => `L:${d.lazy}`);
            
        // Links
        this.svg.append("path")
            .attr("d", `M${cx},40 L${cx - 80},80`)
            .attr("stroke", "var(--border-color)")
            .attr("fill", "none");
            
        this.svg.append("path")
            .attr("d", `M${cx},40 L${cx + 80},80`)
            .attr("stroke", "var(--border-color)")
            .attr("fill", "none");
    }

    animatePushDown() {
        // Animation sequence simulating lazy[node] -> children
        const width = this.svg.node().parentNode.getBoundingClientRect().width;
        const cx = width / 2;

        const pushTokenLeft = this.svg.append("circle")
            .attr("cx", cx).attr("cy", 40).attr("r", 6)
            .attr("fill", "var(--node-lazy)");

        const pushTokenRight = this.svg.append("circle")
            .attr("cx", cx).attr("cy", 40).attr("r", 6)
            .attr("fill", "var(--node-lazy)");

        pushTokenLeft.transition().duration(800)
            .attr("cx", cx - 80).attr("cy", 80)
            .on("end", () => {
                pushTokenLeft.remove();
                this.updateNodeLeft();
            });

        pushTokenRight.transition().duration(800)
            .attr("cx", cx + 80).attr("cy", 80)
            .on("end", () => {
                pushTokenRight.remove();
                this.updateNodeRight();
            });
            
        // Clear root lazy
        this.svg.select(".lazy-node-1 .lazy-tag rect")
            .transition().delay(400).duration(200)
            .attr("fill", "var(--node-default)");
        this.svg.select(".lazy-node-1 .lazy-tag text")
            .transition().delay(400).duration(200)
            .text("L: 0 ");
    }

    updateNodeLeft() {
        this.svg.select(".lazy-node-2 .lazy-tag rect")
            .transition().duration(200)
            .attr("fill", "var(--node-lazy)");
        this.svg.select(".lazy-node-2 .lazy-tag text")
            .text("L:+10");
        this.svg.select(".lazy-node-2 text")
            .text("Sum: 160"); // (120 + 10 * 4) assuming seg size 4
    }

    updateNodeRight() {
        this.svg.select(".lazy-node-3 .lazy-tag rect")
            .transition().duration(200)
            .attr("fill", "var(--node-lazy)");
        this.svg.select(".lazy-node-3 .lazy-tag text")
            .text("L:+10");
        this.svg.select(".lazy-node-3 text")
            .text("Sum: 120"); // (80 + 10 * 4)
    }

    reset() {
        this.initSVG();
    }
}
