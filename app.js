const express = require('express');
const cors = require('cors');
const { exec } = require('child_process');
const fs = require('fs');
const app = express();

app.use(cors());
app.use(express.json());

app.listen(process.env.PORT || 4000);

app.post('/api/findPath', (req, res) => {
    fs.writeFile('temp.csv', req.body.graph, (err) => {
        if (err) throw err;

        req.body.nodes.sort((a, b) => Number(a) - Number(b));

        exec(`./Graph temp.csv ${req.body.term} ${req.body.nodes[0]} ${req.body.nodes[1]}`, (error, stdout, stderr) => {
            if (error) {
                console.error(`exec error: ${error}`);
                return;
            }
            res.json({result: stdout.trim()});
        });
    });
});