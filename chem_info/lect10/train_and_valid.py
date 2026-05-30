import pandas as pd
import rdkit
from rdkit import Chem
from rdkit.Chem import Descriptors
from rdkit.Chem import MACCSkeys
import torch
import numpy as np

file_path = './ci5b00238_si_002.xlsx'

dfs = pd.read_excel(file_path, sheet_name=None, header=1)

# ---------------------------------------------------------
# 1. Clean Training Set (supplementary table S3.1)
# ---------------------------------------------------------
df_train = dfs['supplementary table S3.1'].copy()

# Drop completely empty rows and columns
df_train = df_train.dropna(how='all', axis=1).dropna(how='all', axis=0)

# Remove duplicate entries based on SMILES string
df_train = df_train.drop_duplicates(subset=['Canonical SMILES'], keep='first')
df_train = df_train.reset_index(drop=True)

# ---------------------------------------------------------
# 2. Clean Validation Set (supplementary table S3.2)
# ---------------------------------------------------------
df_test = dfs['supplementary table S3.2'].copy()

# Keep only the first 5 columns (Chemical data)
df_test = df_test.iloc[:, :5].copy()

# Drop rows where 'Chemical name' is NaN and remove duplicates
df_test = df_test.dropna(subset=['Chemical name'])
df_test = df_test.drop_duplicates(subset=['Canonical SMILES'], keep='first')
df_test = df_test.reset_index(drop=True)

def get_fingerprint(smiles):
    mol = Chem.MolFromSmiles(smiles)
    if mol is not None:
        return np.array(MACCSkeys.GenMACCSKeys(mol))
    else:
        return None
    
df_train['Fingerprint'] = df_train['Canonical SMILES'].apply(get_fingerprint)
df_test['Fingerprint'] = df_test['Canonical SMILES'].apply(get_fingerprint)

X_train = torch.tensor(np.stack(df_train['Fingerprint'].values), dtype=torch.float32)
X_test = torch.tensor(np.stack(df_test['Fingerprint'].values), dtype=torch.float32)

Y_train = df_train['Label'].values
Y_test = df_test['Label'].values

train_dataset = torch.utils.data.TensorDataset(X_train, torch.tensor(Y_train, dtype=torch.float32))
test_dataset = torch.utils.data.TensorDataset(X_test, torch.tensor(Y_test, dtype=torch.float32))
train_loader = torch.utils.data.DataLoader(train_dataset, batch_size=32, shuffle=True)
test_loader = torch.utils.data.DataLoader(test_dataset, batch_size=32, shuffle=False)

class SAE(torch.nn.Module):
    def __init__(self, input_dim, hidden_dim):
        super(SAE, self).__init__()
        self.encoder = torch.nn.Sequential(
            torch.nn.Linear(input_dim, 32),
            torch.nn.ReLU(),
            torch.nn.Linear(32, hidden_dim)
        )
        self.decoder = torch.nn.Sequential(
            torch.nn.Linear(hidden_dim, 32),
            torch.nn.ReLU(),
            torch.nn.Linear(32, input_dim),
            torch.nn.Sigmoid()
        )
        self.classifier = torch.nn.Sequential(
            torch.nn.Linear(hidden_dim, 16),
            torch.nn.ReLU(),
            torch.nn.Linear(16, 1),
            torch.nn.Sigmoid()
        )
    
    def reconstruct(self, x):
        encoded = self.encoder(x)
        decoded = self.decoder(encoded)
        return decoded
    
    def classify(self, x):
        encoded = self.encoder(x)
        output = self.classifier(encoded)
        return output.squeeze()
    
input_dim = X_train.shape[1]
hidden_dim = 16
model = SAE(input_dim, hidden_dim)
criterion_recon = torch.nn.MSELoss()
criterion_class = torch.nn.BCELoss()
optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

for epoch in range(50):
    model.train()
    total_loss = 0
    for X_batch, Y_batch in train_loader:
        optimizer.zero_grad()
        recon = model.reconstruct(X_batch)
        loss_recon = criterion_recon(recon, X_batch)
        loss = loss_recon
        loss.backward()
        optimizer.step()
        total_loss += loss.item()
    print(f'Epoch {epoch+1}, Loss: {total_loss/len(train_loader):.4f}')

for epoch in range(50):
    model.train()
    total_loss = 0
    for X_batch, Y_batch in train_loader:
        optimizer.zero_grad()
        class_output = model.classify(X_batch)
        class_output = class_output.reshape(-1)
        loss_class = criterion_class(class_output, Y_batch)
        loss = loss_class
        loss.backward()
        optimizer.step()
        total_loss += loss.item()
    print(f'Epoch {epoch+1}, Loss: {total_loss/len(train_loader):.4f}')

model.eval()
with torch.no_grad():
    correct = 0
    total = 0
    loss = 0
    for X_batch, Y_batch in test_loader:
        class_output = model.classify(X_batch)
        predicted = (class_output > 0.5).float()
        total += Y_batch.size(0)
        correct += (predicted == Y_batch).sum().item()
        loss += criterion_class(class_output, Y_batch).item()
    print(f'Validation Accuracy: {correct/total:.4f}')
    print(f'Validation Loss: {loss/len(test_loader):.4f}')
    