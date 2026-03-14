// segment_tree_viz.js
// Visualizes the segment tree and animates query execution paths

class SegmentTreeViz {
    constructor(containerId) {
        this.container = d3.select(containerId);
        this.treeData = this.buildMockTree(0, 15); // Mock 16-element tree for UI
        this.animationQueue = [];
        this.isPlaying = false;
        this.initSVG();
    }

    buildMockTree(start, end, nodeType = 1) {
        const node = {
            id: nodeType,
            start: start,
            end: end,
            name: `[${start}-${end}]`,
            value: Math.floor(Math.random() * 50000)
        };
        
        if (start < end) {
            const mid = Math.floor((start + end) / 2);
            node.children = [
                this.buildMockTree(start, mid, nodeType * 2),
                this.buildMockTree(mid + 1, end, nodeType * 2 + 1)
            ];
            node.value = node.children[0].value + node.children[1].value;
        }
        return node;
    }

    initSVG() {
        const node = this.container.node();
        if (!node) return;
        
        const rect = node.getBoundingClientRect();
        this.margin = { top: 40, right: 10, bottom: 20, left: 10 };
        this.width = rect.width - this.margin.left - this.margin.right;
        this.height = rect.height - this.margin.top - this.margin.bottom;

        this.container.selectAll("*").remove();

        this.svg = this.container.append("svg")
            .attr("width", this.width + this.margin.left + this.margin.right)
            .attr("height", this.height + this.margin.top + this.margin.bottom)
            .append("g")
            .attr("transform", `translate(${this.margin.left},${this.margin.top})`);

        this.drawTree();
    }

    drawTree() {
        // Compute tree layout
        const treemap = d3.tree().size([this.width, this.height - 60]);
        const root = d3.hierarchy(this.treeData, d => d.children);
        
        this.nodesData = treemap(root);
        
        // Draw Links
        this.svg.selectAll(".link")
            .data(this.nodesData.descendants().slice(1))
            .enter().append("path")
            .attr("class", "link")
            .attr("d", d => {
                return "M" + d.x + "," + d.y
                     + "C" + d.x + "," + (d.y + d.parent.y) / 2
                     + " " + d.parent.x + "," +  (d.y + d.parent.y) / 2
                     + " " + d.parent.x + "," + d.parent.y;
            });
            
        // Draw Nodes
        const nodeGrp = this.svg.selectAll(".node")
            .data(this.nodesData.descendants())
            .enter().append("g")
            .attr("class", "node default")
            .attr("id", d => `node-${d.data.id}`)
            .attr("transform", d => `translate(${d.x},${d.y})`);
            
        nodeGrp.append("rect")
            .attr("width", 50)
            .attr("height", 36)
            .attr("x", -25)
            .attr("y", -18);
            
        nodeGrp.append("text")
            .attr("dy", "-2")
            .attr("text-anchor", "middle")
            .text(d => d.data.value);
            
        nodeGrp.append("text")
            .attr("class", "range")
            .attr("dy", "12")
            .attr("text-anchor", "middle")
            .text(d => d.data.name);
    }

    reset() {
        this.svg.selectAll(".node")
            .attr("class", "node default");
        this.isPlaying = false;
        this.animationQueue = [];
    }

    animateQuery(queryLow, queryHigh, speedMs) {
        this.reset();
        
        // Mock query path generation
        const paths = [];
        
        const traverse = (node, start, end) => {
            if (end < queryLow || start > queryHigh) {
                paths.push({ id: node.data.id, type: 'default' });
                return;
            }
            if (queryLow <= start && end <= queryHigh) {
                paths.push({ id: node.data.id, type: 'full' });
                return;
            }
            paths.push({ id: node.data.id, type: 'partial' });
            if (node.children) {
                const mid = Math.floor((start + end) / 2);
                traverse(node.children[0], start, mid);
                traverse(node.children[1], mid + 1, end);
            }
        };
        
        traverse(this.nodesData, 0, 15);
        
        // Execute animation step by step
        this.animationQueue = paths;
        this.isPlaying = true;
        this.processQueue(speedMs);
    }
    
    processQueue(speedMs) {
        if (!this.isPlaying || this.animationQueue.length === 0) {
            this.isPlaying = false;
            return;
        }
        
        const step = this.animationQueue.shift();
        
        // Update DOM
        const targetNode = d3.select(`#node-${step.id}`);
        // Highlight active briefly
        targetNode.attr("class", "node active");
        
        setTimeout(() => {
            targetNode.attr("class", `node ${step.type}`);
            setTimeout(() => this.processQueue(speedMs), parseInt(speedMs) / 2);
        }, parseInt(speedMs) / 2);
    }
}
