# app.py - Flask backend
from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess, os

app = Flask(__name__)
CORS(app) # allow browser requests from index.html

# WINDOWS ADJUSTMENT: Ensure we point to the .exe compiled by MinGW
BINARY = os.path.join(os.path.dirname(__file__), "org_chart.exe")

@app.route("/traverse", methods=["POST"])
def traverse():
    data = request.get_json()
    inp = data.get("input", "").strip()
    
    if not inp:
        return jsonify({"error": "Empty input"}), 400
        
    # Call the C binary using subprocess, passing the input string via stdin
    result = subprocess.run(
        [BINARY],
        input=inp,
        capture_output=True, text=True, timeout=5
    )
    
    if result.returncode != 0:
        return jsonify({"error": result.stderr}), 500
        
    # Return the stdout (which contains your org chart and stats) as JSON
    return jsonify({"result": result.stdout})

if __name__ == "__main__":
    app.run(debug=True, port=5000)